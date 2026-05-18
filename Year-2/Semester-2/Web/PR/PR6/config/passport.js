// Passport.js: LocalStrategy з bcrypt-перевіркою + захист від брутфорсу (account lockout 15хв).
const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;
const bcrypt = require('bcrypt');
const { stmts, audit } = require('../db');

const LOCKOUT_MS = 15 * 60 * 1000;
const LOCKOUT_THRESHOLD = 5;

passport.use(new LocalStrategy(
    { usernameField: 'email', passwordField: 'password', passReqToCallback: true },
    async (req, email, password, done) => {
        try {
            const user = stmts.findUserByEmail.get(email);
            if (!user) {
                audit(null, 'login_failed', req.ip, `unknown:${email}`);
                return done(null, false, { message: 'Невірний email або пароль' });
            }
            if (user.locked_until && user.locked_until > Date.now()) {
                audit(user.id, 'login_blocked_locked', req.ip);
                return done(null, false, { message: 'Акаунт тимчасово заблоковано. Спробуйте за 15 хв.' });
            }
            const ok = await bcrypt.compare(password, user.password_hash);
            if (!ok) {
                stmts.incFailed.run(user.id);
                const fresh = stmts.findUserById.get(user.id);
                if (fresh.failed_logins >= LOCKOUT_THRESHOLD) {
                    stmts.lockUser.run(Date.now() + LOCKOUT_MS, user.id);
                    audit(user.id, 'account_locked', req.ip);
                }
                audit(user.id, 'login_failed', req.ip, `bad_password attempt=${fresh.failed_logins}`);
                return done(null, false, { message: 'Невірний email або пароль' });
            }
            if (!user.email_verified) {
                audit(user.id, 'login_blocked_unverified', req.ip);
                return done(null, false, { message: 'Email не підтверджено. Перевірте поштову скриньку.' });
            }
            stmts.resetFailed.run(user.id);
            audit(user.id, 'login_success', req.ip);
            return done(null, user);
        } catch (err) {
            return done(err);
        }
    }
));

// Serialize → у сесію кладемо лише id (не пароль і не повний об'єкт).
passport.serializeUser((user, done) => done(null, user.id));

passport.deserializeUser((id, done) => {
    try {
        const user = stmts.findUserById.get(id);
        if (!user) return done(null, false);
        // Не повертаємо хеш паролю в req.user.
        const { password_hash, verify_token, ...safe } = user;
        return done(null, safe);
    } catch (err) {
        return done(err);
    }
});

module.exports = passport;
