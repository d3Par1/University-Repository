// Заповнення БД демо-даними: 3 користувачі (по одній ролі), 6 квартир (3 за resident).
// Запускати лише у dev: видаляє та перестворює БД, якщо передано --reset.
const fs = require('fs');
const path = require('path');
const bcrypt = require('bcrypt');

const DB_FILE = process.env.DB_FILE || path.join(__dirname, 'data', 'heatpump.db');
if (process.argv.includes('--reset') && fs.existsSync(DB_FILE)) {
    fs.rmSync(DB_FILE, { force: true });
    for (const ext of ['-shm', '-wal']) {
        if (fs.existsSync(DB_FILE + ext)) fs.rmSync(DB_FILE + ext, { force: true });
    }
    console.log('▶ DB скинуто.');
}

const { db, stmts } = require('./db');

(async () => {
    const existing = stmts.countUsers.get();
    if (existing.n > 0 && !process.argv.includes('--force')) {
        console.log(`✓ Уже є ${existing.n} користувачів — пропускаю. (--reset для очищення)`);
        process.exit(0);
    }

    const hash = await bcrypt.hash('Demo12345!', 10);
    const insert = db.prepare(`INSERT INTO users (email, password_hash, name, role, email_verified, verify_token, created_at)
                               VALUES (?, ?, ?, ?, 1, NULL, ?)`);
    const r1 = insert.run('resident@demo.local',  hash, 'Іван Жилець',          'resident',         Date.now());
    const r2 = insert.run('tech@demo.local',      hash, 'Олег Технік',          'technician',       Date.now());
    const r3 = insert.run('manager@demo.local',   hash, 'Марія Менеджер',       'facility_manager', Date.now());

    const insertApt = db.prepare(`INSERT INTO apartments (number, floor, area_m2, owner_user_id, target_temp, current_temp, cop, power_kw)
                                  VALUES (?, ?, ?, ?, ?, ?, ?, ?)`);
    const apts = [
        ['12А', 1, 42.5, r1.lastInsertRowid, 21.0, 20.8, 3.9, 0.5],
        ['18Б', 2, 56.0, r1.lastInsertRowid, 22.0, 21.5, 3.8, 0.8],
        ['25В', 3, 73.0, r1.lastInsertRowid, 21.5, 21.2, 4.0, 0.6],
        ['31А', 4, 45.0, null, 20.0, 19.8, 3.7, 0.3],
        ['44Б', 5, 62.5, null, 22.5, 22.0, 4.1, 0.7],
        ['56В', 6, 88.0, null, 21.0, 20.6, 3.6, 0.9]
    ];
    for (const a of apts) insertApt.run(...a);

    console.log('✓ Засіяно:');
    console.log('   resident@demo.local / Demo12345!  → 3 квартири');
    console.log('   tech@demo.local     / Demo12345!');
    console.log('   manager@demo.local  / Demo12345!');
})();
