// RBAC + ownership middleware.
const { stmts, audit } = require('../db');

function isAuthenticated(req, res, next) {
    if (req.isAuthenticated && req.isAuthenticated()) return next();
    audit(null, 'unauthorized_access', req.ip, `${req.method} ${req.originalUrl}`);
    // API → завжди JSON 401. HTML-сторінки (/dashboard тощо) → редірект на /login.
    if (req.originalUrl.startsWith('/api') || req.xhr || req.is('application/json')) {
        return res.status(401).json({ error: 'Необхідна автентифікація' });
    }
    return res.redirect('/login');
}

// hasRole('technician','facility_manager')
function hasRole(...roles) {
    return (req, res, next) => {
        if (!req.isAuthenticated()) {
            return res.status(401).json({ error: 'Необхідна автентифікація' });
        }
        if (!roles.includes(req.user.role)) {
            audit(req.user.id, 'forbidden', req.ip, `role=${req.user.role} needed=${roles.join(',')} ${req.originalUrl}`);
            return res.status(403).json({ error: 'Недостатньо прав доступу' });
        }
        return next();
    };
}

// Resident може звертатися лише до своїх квартир. Technician/manager — до всіх.
function ownsApartmentOrPrivileged(req, res, next) {
    const id = parseInt(req.params.id, 10);
    if (!Number.isFinite(id)) return res.status(400).json({ error: 'Некоректний id квартири' });
    const apt = stmts.apartmentById.get(id);
    if (!apt) return res.status(404).json({ error: 'Квартиру не знайдено' });
    const privileged = ['technician', 'facility_manager'].includes(req.user.role);
    const owner = apt.owner_user_id === req.user.id;
    if (!privileged && !owner) {
        audit(req.user.id, 'ownership_violation', req.ip, `apartment_id=${id}`);
        return res.status(403).json({ error: 'Ця квартира не належить вам' });
    }
    req.apartment = apt;
    return next();
}

module.exports = { isAuthenticated, hasRole, ownsApartmentOrPrivileged };
