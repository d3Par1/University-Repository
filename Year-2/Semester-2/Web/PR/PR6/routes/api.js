// API ендпоінти Варіанту 15. Усі під автентифікацією + RBAC + (де треба) ownership.
const express = require('express');
const { body, validationResult } = require('express-validator');
const { stmts, audit } = require('../db');
const { isAuthenticated, hasRole, ownsApartmentOrPrivileged } = require('../middleware/auth');

const router = express.Router();

// Усе /api/* — лише для автентифікованих.
router.use(isAuthenticated);

// GET /api/me — поточний користувач (зручно для клієнта).
router.get('/me', (req, res) => {
    const { id, email, name, role } = req.user;
    res.json({ id, email, name, role });
});

// GET /api/apartments
// resident: лише свої. technician/facility_manager: усі.
router.get('/apartments', (req, res) => {
    if (req.user.role === 'resident') {
        return res.json(stmts.apartmentsByOwner.all(req.user.id));
    }
    return res.json(stmts.listApartments.all());
});

// POST /api/heatpump/temperature  { apartmentId, target } — resident своєї, або technician/manager.
router.post('/heatpump/temperature',
    body('apartmentId').isInt({ min: 1 }),
    body('target').isFloat({ min: 12, max: 28 }).withMessage('Цільова температура 12–28°C'),
    (req, res) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) return res.status(400).json({ errors: errors.array() });

        const id = parseInt(req.body.apartmentId, 10);
        const apt = stmts.apartmentById.get(id);
        if (!apt) return res.status(404).json({ error: 'Квартиру не знайдено' });

        const privileged = ['technician', 'facility_manager'].includes(req.user.role);
        const owner = apt.owner_user_id === req.user.id;
        if (!privileged && !owner) {
            audit(req.user.id, 'ownership_violation', req.ip, `apartment_id=${id}`);
            return res.status(403).json({ error: 'Ця квартира не належить вам' });
        }

        const target = parseFloat(req.body.target);
        stmts.updateApartmentTarget.run(target, id);
        stmts.insertTempLog.run(id, req.user.id, apt.target_temp, target, Date.now());
        audit(req.user.id, 'temperature_set', req.ip, `apt=${apt.number} ${apt.target_temp}→${target}`);
        return res.json({ ok: true, apartmentId: id, target_temp: target });
    }
);

// POST /api/maintenance/schedule — лише technician.
router.post('/maintenance/schedule',
    hasRole('technician'),
    body('apartmentId').optional({ nullable: true }).isInt({ min: 1 }),
    body('title').trim().isLength({ min: 3, max: 120 }).escape(),
    body('notes').optional().trim().isLength({ max: 1000 }).escape(),
    body('scheduledFor').isISO8601().toDate(),
    (req, res) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) return res.status(400).json({ errors: errors.array() });
        const aptId = req.body.apartmentId ? parseInt(req.body.apartmentId, 10) : null;
        const info = stmts.insertMaintenance.run(
            aptId, req.user.id, req.body.title, req.body.notes || null,
            req.body.scheduledFor.getTime(), Date.now()
        );
        audit(req.user.id, 'maintenance_scheduled', req.ip, `id=${info.lastInsertRowid}`);
        res.status(201).json({ ok: true, id: info.lastInsertRowid });
    }
);

router.get('/maintenance', hasRole('technician', 'facility_manager'), (req, res) => {
    res.json(stmts.listMaintenance.all());
});

// GET /api/system/efficiency — лише facility_manager. Агрегати по всьому будинку.
router.get('/system/efficiency', hasRole('facility_manager'), (req, res) => {
    const apartments = stmts.listApartments.all();
    const totals = stmts.avgCop.get();
    const breakdown = stmts.countMaintenance.all();
    const totalArea = apartments.reduce((s, a) => s + a.area_m2, 0);
    const totalPower = apartments.reduce((s, a) => s + a.power_kw, 0);
    const heatedArea = apartments.length;
    res.json({
        apartments_count: heatedArea,
        total_area_m2: +totalArea.toFixed(1),
        total_electrical_kw: +totalPower.toFixed(2),
        thermal_kw_est: +(totalPower * (totals.avg_cop || 1)).toFixed(2),
        avg_cop: +(totals.avg_cop || 0).toFixed(2),
        maintenance_breakdown: breakdown
    });
});

// GET /api/audit — лише facility_manager.
router.get('/audit', hasRole('facility_manager'), (req, res) => {
    res.json(stmts.listAudit.all());
});

module.exports = router;
