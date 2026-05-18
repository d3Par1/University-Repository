// Реєстрація / логін / логаут / статус / верифікація email.
const express = require('express');
const bcrypt = require('bcrypt');
const crypto = require('crypto');
const rateLimit = require('express-rate-limit');
const { body, validationResult } = require('express-validator');
const passport = require('../config/passport');
const { stmts, audit } = require('../db');

const router = express.Router();

const BCRYPT_ROUNDS = parseInt(process.env.BCRYPT_ROUNDS, 10) || 10;

// Спеціальний жорсткий ліміт лише на /auth/login — anti brute-force.
const loginLimiter = rateLimit({
    windowMs: parseInt(process.env.LOGIN_RATE_WINDOW_MS, 10) || 15 * 60 * 1000,
    max:      parseInt(process.env.LOGIN_RATE_MAX, 10) || 5,
    standardHeaders: true,
    legacyHeaders: false,
    keyGenerator: (req) => `${req.ip}|${(req.body?.email || '').toLowerCase()}`,
    message: { error: 'Забагато спроб входу. Спробуйте за 15 хвилин.' }
});

// Менш суворий ліміт на реєстрацію — anti account-flooding.
const registerLimiter = rateLimit({
    windowMs: 60 * 60 * 1000,
    max: 10,
    standardHeaders: true,
    legacyHeaders: false,
    message: { error: 'Забагато реєстрацій з цієї IP. Спробуйте за годину.' }
});

// Валідатори express-validator: типи + санітизація.
const registerValidators = [
    body('email').isEmail().withMessage('Невірний формат email').normalizeEmail(),
    body('password')
        .isLength({ min: 10 }).withMessage('Пароль має бути не менше 10 символів')
        .matches(/[A-Za-z]/).withMessage('Пароль має містити літери')
        .matches(/\d/).withMessage('Пароль має містити цифри'),
    body('name').trim().isLength({ min: 2, max: 80 }).withMessage('Імʼя 2–80 символів').escape(),
    body('role').isIn(['resident', 'technician', 'facility_manager']).withMessage('Невірна роль')
];

router.post('/register', registerLimiter, registerValidators, async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(400).json({ errors: errors.array() });
    }
    try {
        const { email, password, name, role } = req.body;
        const existing = stmts.findUserByEmail.get(email);
        if (existing) {
            return res.status(409).json({ error: 'Користувач з таким email вже зареєстрований' });
        }
        const hash = await bcrypt.hash(password, BCRYPT_ROUNDS);
        const verifyToken = crypto.randomBytes(24).toString('hex');
        const info = stmts.insertUser.run(email, hash, name, role, 0, verifyToken, Date.now());
        audit(info.lastInsertRowid, 'register', req.ip, `role=${role}`);
        // У реальній системі тут надсилається email. Для лабораторної повертаємо токен у відповіді.
        return res.status(201).json({
            message: 'Реєстрація успішна. Підтвердіть email за посиланням.',
            verify_url: `/verify?token=${verifyToken}`,
            user: { id: info.lastInsertRowid, email, name, role }
        });
    } catch (err) {
        return res.status(500).json({ error: 'Помилка реєстрації' });
    }
});

router.post('/login', loginLimiter, (req, res, next) => {
    passport.authenticate('local', (err, user, info) => {
        if (err) return next(err);
        if (!user) return res.status(401).json({ error: info?.message || 'Невірні дані' });
        // КРИТИЧНО: регенеруємо session id після успішного логіну → захист від session-fixation.
        req.session.regenerate((regenErr) => {
            if (regenErr) return next(regenErr);
            req.login(user, (loginErr) => {
                if (loginErr) return next(loginErr);
                return res.json({
                    message: 'Вхід успішний',
                    user: { id: user.id, email: user.email, name: user.name, role: user.role }
                });
            });
        });
    })(req, res, next);
});

router.post('/logout', (req, res, next) => {
    const uid = req.user?.id;
    req.logout((err) => {
        if (err) return next(err);
        req.session.destroy(() => {
            res.clearCookie('hp.sid');
            audit(uid, 'logout', req.ip);
            res.json({ message: 'Вихід успішний' });
        });
    });
});

router.get('/status', (req, res) => {
    if (req.isAuthenticated()) {
        const { id, email, name, role } = req.user;
        return res.json({ authenticated: true, user: { id, email, name, role } });
    }
    return res.json({ authenticated: false });
});

module.exports = router;
