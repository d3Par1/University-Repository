# Практична робота №6 — Захищений портал керування тепловими насосами

**Курс:** Основи Веб-програмування
**Варіант:** 15 — Портал керування тепловими насосами житлового комплексу
**Група:** ТВ-43
**Семестр:** 2-й, 2025/26 н.р.

## Опис

Веб-портал для керування мережею теплових насосів багатоквартирного будинку з повним стеком засобів безпеки: автентифікація через Passport.js (LocalStrategy + bcrypt), RBAC з трьома ролями (`resident` / `technician` / `facility_manager`), захист від XSS (CSP + EJS-екранування + express-validator.escape), CSRF-токени (csurf), параметризовані SQL-запити (`node:sqlite`), rate-limiting (per-IP + per-account lockout), валідація даних (express-validator) і helmet HTTP-заголовки.

## Архітектура

```
┌──────────── БРАУЗЕР ─────────────┐
│  EJS rendered HTML + Bootstrap   │
│  ┌── fetch /api/* (CSRF header)──┐
│  └── form POST /auth/* ──────────┘
└────────────│─────────────────────┘
             ▼
┌────── Node.js Express ───────────┐
│ helmet → bodyParser → cookies →  │
│ session → passport → csurf →     │
│ routes (auth.js + api.js) →      │
│ middleware (isAuth/hasRole/own)  │
│                                  │
│ node:sqlite (parameterized) +    │
│ simulator (тік 5с)               │
└──────────────────────────────────┘
```

## Ролі та матриця прав доступу

| Endpoint                          | resident         | technician | facility_manager |
|-----------------------------------|------------------|------------|------------------|
| POST /auth/register               | ✓ (на себе)      | ✓          | ✓                |
| POST /auth/login                  | ✓                | ✓          | ✓                |
| POST /auth/logout                 | ✓                | ✓          | ✓                |
| GET  /api/me                      | ✓                | ✓          | ✓                |
| GET  /api/apartments              | свої             | усі        | усі              |
| POST /api/heatpump/temperature    | свої             | усі        | усі              |
| POST /api/maintenance/schedule    | ✗ (403)          | ✓          | ✗ (403)          |
| GET  /api/maintenance             | ✗                | ✓          | ✓                |
| GET  /api/system/efficiency       | ✗ (403)          | ✗ (403)    | ✓                |
| GET  /api/audit                   | ✗ (403)          | ✗ (403)    | ✓                |

## Запуск

```bash
npm install
npm run seed      # створити БД з тестовими акаунтами (--reset для очищення)
npm start         # http://localhost:3000/
npm test          # автотести безпеки у другому терміналі
```

**Тестові акаунти** (пароль для всіх: `Demo12345!`):
- `resident@demo.local` — 3 квартири
- `tech@demo.local` — технік
- `manager@demo.local` — менеджер комплексу

## Структура проєкту

```
PR6/
├── server.js                    — Express + middleware stack + start
├── db.js                        — node:sqlite + prepared statements
├── simulator.js                 — тік 5с, наближає current_temp до target
├── seed.js                      — посів демо-даних
├── test-api.js                  — автотести 26 сценаріїв безпеки
├── take-screenshots.js          — Puppeteer-core
├── config/passport.js           — LocalStrategy + bcrypt + lockout
├── routes/auth.js               — register, login, logout, verify
├── routes/api.js                — /api/* з RBAC + ownership
├── middleware/auth.js           — isAuthenticated, hasRole, ownsApartment
├── views/                       — EJS-шаблони (login, register, dashboard)
├── public/                      — styles.css, login.js, register.js, dashboard.js
├── data/heatpump.db             — SQLite БД
├── screenshots/                 — скріншоти роботи додатку
├── ZVIT_PR6.md                  — повний звіт
└── README.md
```

## Технології

- **Сервер:** Node.js 20+ (для `node:sqlite`), Express 4, Passport.js, csurf, helmet, express-rate-limit
- **БД:** `node:sqlite` (вбудований у Node 22+) з параметризованими prepared statements
- **Хешування:** bcrypt, 10 rounds
- **Валідація:** express-validator (email, length, regex, escape)
- **Клієнт:** EJS, Bootstrap 5.3, vanilla JS (fetch з CSRF-заголовком)
- **Тести:** власні fetch-based сценарії (26 перевірок)

## Реалізовані вимоги Варіанту 15

| Вимога з PDF                                    | Реалізація                                            |
|-------------------------------------------------|-------------------------------------------------------|
| Реєстрація мешканців та техніків                | `POST /auth/register` з role-enum, валідацією, верифікацією email |
| Налаштування температурних режимів              | `POST /api/heatpump/temperature` з валідацією 12–28°C |
| Жителі керують своїми квартирами                | `ownsApartmentOrPrivileged` middleware                |
| Технік проводить обслуговування                 | `POST /api/maintenance/schedule` (hasRole technician) |
| Менеджер оптимізує загальну систему             | `GET /api/system/efficiency` + `/api/audit`           |
| Password strength meter                         | Client-side meter + server `isLength({min:10})` + regex |
| Account email verification                      | Token-based, посилання `/verify?token=…`              |
| Remember me functionality (secure)              | Сесія з `httpOnly`, `sameSite=strict`, 2-год TTL      |
| Session hijacking prevention                    | `req.session.regenerate()` після логіну               |

## Покриття 30-балової частини «Безпека»

| Захист                  | Реалізація                                                |
|-------------------------|-----------------------------------------------------------|
| Хешування паролів       | bcrypt 10 rounds (`config/passport.js`)                   |
| XSS                     | CSP `default-src 'self'`, EJS `<%= %>`, validator.escape  |
| CSRF                    | csurf для всіх не-GET, токен у meta + form _csrf          |
| Валідація / санітизація | express-validator: `isEmail`, `isLength`, `matches`, `escape` |
| Rate limiting           | 120 req/min на /api, 5 спроб/15хв на /auth/login          |
| Інші: lockout, helmet, sameSite cookie, parameterized SQL, audit log | див. server.js + passport.js |
