// node:sqlite (вбудований у Node 22+). Параметризовані prepared statements → захист від SQL-ін'єкцій.
const { DatabaseSync } = require('node:sqlite');
const path = require('path');
const fs = require('fs');

const DB_FILE = process.env.DB_FILE || path.join(__dirname, 'data', 'heatpump.db');
fs.mkdirSync(path.dirname(DB_FILE), { recursive: true });

const db = new DatabaseSync(DB_FILE);
db.exec('PRAGMA journal_mode = WAL');
db.exec('PRAGMA foreign_keys = ON');

db.exec(`
CREATE TABLE IF NOT EXISTS users (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    email           TEXT UNIQUE NOT NULL,
    password_hash   TEXT NOT NULL,
    name            TEXT NOT NULL,
    role            TEXT NOT NULL CHECK(role IN ('resident','technician','facility_manager')),
    email_verified  INTEGER NOT NULL DEFAULT 0,
    verify_token    TEXT,
    failed_logins   INTEGER NOT NULL DEFAULT 0,
    locked_until    INTEGER,
    created_at      INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS apartments (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    number          TEXT UNIQUE NOT NULL,
    floor           INTEGER NOT NULL,
    area_m2         REAL NOT NULL,
    owner_user_id   INTEGER REFERENCES users(id) ON DELETE SET NULL,
    target_temp     REAL NOT NULL DEFAULT 21.0,
    current_temp    REAL NOT NULL DEFAULT 21.0,
    cop             REAL NOT NULL DEFAULT 3.8,
    power_kw        REAL NOT NULL DEFAULT 0.0
);

CREATE TABLE IF NOT EXISTS temperature_log (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    apartment_id    INTEGER NOT NULL REFERENCES apartments(id),
    user_id         INTEGER REFERENCES users(id),
    old_temp        REAL,
    new_temp        REAL NOT NULL,
    changed_at      INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS maintenance (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    apartment_id    INTEGER REFERENCES apartments(id),
    technician_id   INTEGER REFERENCES users(id),
    title           TEXT NOT NULL,
    notes           TEXT,
    scheduled_for   INTEGER NOT NULL,
    status          TEXT NOT NULL DEFAULT 'scheduled' CHECK(status IN ('scheduled','done','cancelled')),
    created_at      INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS audit_log (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER,
    action          TEXT NOT NULL,
    ip              TEXT,
    detail          TEXT,
    at              INTEGER NOT NULL
);
`);

const stmts = {
    findUserByEmail: db.prepare('SELECT * FROM users WHERE email = ?'),
    findUserById:    db.prepare('SELECT * FROM users WHERE id = ?'),
    findByVerifyToken: db.prepare('SELECT * FROM users WHERE verify_token = ?'),
    insertUser:      db.prepare(`INSERT INTO users (email, password_hash, name, role, email_verified, verify_token, created_at)
                                 VALUES (?, ?, ?, ?, ?, ?, ?)`),
    setVerified:     db.prepare('UPDATE users SET email_verified = 1, verify_token = NULL WHERE verify_token = ?'),
    incFailed:       db.prepare('UPDATE users SET failed_logins = failed_logins + 1 WHERE id = ?'),
    lockUser:        db.prepare('UPDATE users SET locked_until = ?, failed_logins = 0 WHERE id = ?'),
    resetFailed:     db.prepare('UPDATE users SET failed_logins = 0, locked_until = NULL WHERE id = ?'),

    listApartments:           db.prepare('SELECT * FROM apartments ORDER BY number'),
    apartmentsByOwner:        db.prepare('SELECT * FROM apartments WHERE owner_user_id = ? ORDER BY number'),
    apartmentById:            db.prepare('SELECT * FROM apartments WHERE id = ?'),
    updateApartmentTarget:    db.prepare('UPDATE apartments SET target_temp = ? WHERE id = ?'),
    updateApartmentCurrent:   db.prepare('UPDATE apartments SET current_temp = ?, power_kw = ? WHERE id = ?'),
    insertTempLog:            db.prepare(`INSERT INTO temperature_log (apartment_id, user_id, old_temp, new_temp, changed_at)
                                          VALUES (?, ?, ?, ?, ?)`),
    listMaintenance:          db.prepare(`SELECT m.*, a.number AS apt_number FROM maintenance m
                                          LEFT JOIN apartments a ON a.id = m.apartment_id
                                          ORDER BY scheduled_for DESC LIMIT 50`),
    insertMaintenance:        db.prepare(`INSERT INTO maintenance (apartment_id, technician_id, title, notes, scheduled_for, status, created_at)
                                          VALUES (?, ?, ?, ?, ?, 'scheduled', ?)`),

    insertAudit:              db.prepare('INSERT INTO audit_log (user_id, action, ip, detail, at) VALUES (?, ?, ?, ?, ?)'),
    listAudit:                db.prepare('SELECT * FROM audit_log ORDER BY at DESC LIMIT 100'),

    countUsers:               db.prepare('SELECT COUNT(*) AS n FROM users'),
    avgCop:                   db.prepare('SELECT AVG(cop) AS avg_cop, SUM(power_kw) AS total_kw FROM apartments'),
    countMaintenance:         db.prepare(`SELECT status, COUNT(*) AS n FROM maintenance GROUP BY status`)
};

function audit(userId, action, ip, detail) {
    stmts.insertAudit.run(userId ?? null, action, ip ?? null, detail ?? null, Date.now());
}

module.exports = { db, stmts, audit, DB_FILE };
