// Express + Passport + helmet + csurf + rate-limit + sessions.
// Порядок middleware ВАЖЛИВИЙ: helmet → body parsers → cookies → session → passport → csurf → routes.
const path = require('path');
const express = require('express');
const session = require('express-session');
const cookieParser = require('cookie-parser');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const csrf = require('csurf');
const crypto = require('crypto');

const passport = require('./config/passport');
const { stmts, audit } = require('./db');
const simulator = require('./simulator');

const app = express();
const PORT = parseInt(process.env.PORT, 10) || 3000;
const SESSION_SECRET = process.env.SESSION_SECRET || crypto.randomBytes(32).toString('hex');

// 1. Безпекові HTTP-заголовки + CSP.
app.use(helmet({
    contentSecurityPolicy: {
        directives: {
            defaultSrc: ["'self'"],
            scriptSrc: ["'self'", "https://cdn.jsdelivr.net"],
            styleSrc:  ["'self'", "https://cdn.jsdelivr.net", "'unsafe-inline'"], // unsafe-inline лише для Bootstrap utility-стилів
            imgSrc:    ["'self'", "data:"],
            connectSrc: ["'self'"],
            objectSrc: ["'none'"],
            frameAncestors: ["'none'"]
        }
    },
    referrerPolicy: { policy: 'no-referrer' },
    crossOriginEmbedderPolicy: false
}));

// 2. Парсери (з лімітом розміру → захист від parser-DoS).
app.use(express.json({ limit: '64kb' }));
app.use(express.urlencoded({ extended: false, limit: '64kb' }));
app.use(cookieParser(SESSION_SECRET));

// 3. Сесії: httpOnly, sameSite=strict, secure-у-проді, rolling=false.
app.use(session({
    name: 'hp.sid',
    secret: SESSION_SECRET,
    resave: false,
    saveUninitialized: false,
    rolling: false,
    cookie: {
        httpOnly: true,
        sameSite: 'strict',
        secure: process.env.NODE_ENV === 'production',
        maxAge: 1000 * 60 * 60 * 2 // 2 години
    }
}));

// 4. Passport.
app.use(passport.initialize());
app.use(passport.session());

// 5. CSRF — після сесії, перед роутами форм. API ендпоінти все одно під сесією → CSRF на них теж.
const csrfProtection = csrf();

// 6. Глобальний rate-limit для всього API (м'який, проти scraping).
const apiLimiter = rateLimit({
    windowMs: parseInt(process.env.API_RATE_WINDOW_MS, 10) || 60_000,
    max:      parseInt(process.env.API_RATE_MAX, 10) || 120,
    standardHeaders: true,
    legacyHeaders: false,
    message: { error: 'Забагато запитів. Спробуйте пізніше.' }
});
app.use('/api', apiLimiter);

// 7. View engine (EJS) + static.
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'views'));
app.use(express.static(path.join(__dirname, 'public'), { maxAge: '1h' }));

// 8. Helper: прокинути user + csrf token у всі views.
app.use(csrfProtection);
app.use((req, res, next) => {
    res.locals.user = req.user || null;
    res.locals.csrfToken = req.csrfToken();
    next();
});

// 9. Роути.
app.use('/auth', require('./routes/auth'));
app.use('/api',  require('./routes/api'));

// 10. Сторінки.
app.get('/', (req, res) => {
    if (req.isAuthenticated()) return res.redirect('/dashboard');
    return res.redirect('/login');
});
app.get('/login',    (req, res) => res.render('login', { error: null }));
app.get('/register', (req, res) => res.render('register', { error: null, form: {} }));
app.get('/dashboard', require('./middleware/auth').isAuthenticated, (req, res) => res.render('dashboard'));
app.get('/verify',   (req, res) => {
    const token = String(req.query.token || '');
    if (!token) return res.status(400).send('Токен відсутній');
    const result = stmts.setVerified.run(token);
    if (result.changes === 0) return res.status(400).render('login', { error: 'Невірний або вже використаний токен' });
    audit(null, 'email_verified', req.ip, token.slice(0, 8));
    return res.render('login', { error: null, info: 'Email підтверджено. Тепер можна увійти.' });
});

// 11. Помилки CSRF + загальні.
app.use((err, req, res, next) => {
    if (err && err.code === 'EBADCSRFTOKEN') {
        audit(req.user?.id, 'csrf_failed', req.ip, `${req.method} ${req.originalUrl}`);
        return res.status(403).json({ error: 'Невірний CSRF-токен' });
    }
    console.error('[error]', err);
    res.status(500).json({ error: 'Внутрішня помилка сервера' });
});

// 12. Старт.
if (require.main === module) {
    simulator.start();
    app.listen(PORT, () => {
        console.log(`▶  HeatPump security portal: http://localhost:${PORT}/`);
    });
}

module.exports = app;
