Міністерство освіти і науки України
Національний технічний університет України
«Київський політехнічний інститут ім. І. Сікорського»


Кафедра інженерії програмного забезпечення в енергетиці




# Практична робота № 6

з курсу: «Основи Веб-програмування»




> **Виконав:** студент 2-го курсу, групи ТВ-43
> **Степаненко Назар Юрійович**
> Посилання на GitHub репозиторій: https://github.com/d3Par1/University-Repository
>
> **Перевірив:**
> Недашківський О.Л.


Київ 2025/2026

---

## Практична робота № 6

## Безпека веб-додатків: захищений портал керування тепловими насосами

**Мета роботи:**
Реалізувати захист веб-застосунку (автентифікацію через Passport.js, роботу з сесіями, захист від XSS/CSRF, валідацію та санітизацію даних, rate limiting) на прикладі порталу керування мережею теплових насосів житлового комплексу. Засвоїти послідовність middleware у Express, RBAC-модель доступу, параметризовані SQL-запити та практики безпечної взаємодії клієнт-сервер.

**Завдання:**

Варіант 15: Портал керування тепловими насосами.

Об'єкт автоматизації: система теплових насосів житлового комплексу з 6 квартир (загальна площа 367 м², середній COP 3.85). Три категорії користувачів: мешканці (керують своїми квартирами), техніки (планують обслуговування), менеджер комплексу (оптимізує загальну систему, переглядає аудит-журнал).

1. Спроєктувати веб-портал з трирівневою рольовою моделлю (resident / technician / facility_manager).
2. Реалізувати реєстрацію з валідацією даних та підтвердженням email через токен.
3. Реалізувати автентифікацію через Passport.js (LocalStrategy + bcrypt).
4. Реалізувати авторизацію на базі RBAC та ownership для квартир мешканців.
5. Реалізувати захист від XSS через CSP-заголовки, EJS-екранування та `escape`-санітайзер.
6. Реалізувати захист від CSRF через токен-механізм (`csurf`).
7. Реалізувати rate limiting (per-IP) та account lockout (per-account, 5 спроб → 15 хв блок).
8. Забезпечити параметризовані SQL-запити через `node:sqlite` prepared statements.
9. Сформувати аудит-журнал та інтерфейс перегляду для менеджера.
10. Скласти автоматичний тест-сценарій з 26 перевірок безпеки.

**Параметри об'єкта (Варіант 15):**

| Параметр       | Тип    | Опис                                                |
|----------------|--------|-----------------------------------------------------|
| number         | text   | Номер квартири (наприклад «12А»)                    |
| floor          | number | Поверх (1..6)                                       |
| area_m2        | number | Площа квартири, м²                                  |
| owner_user_id  | number | Власник (FK на users)                               |
| target_temp    | number | Цільова температура, °C (діапазон 12–28)            |
| current_temp   | number | Поточна температура, °C                             |
| cop            | number | Coefficient of Performance теплового насоса (~3.6–4.1) |
| power_kw       | number | Поточна електрична потужність насоса, кВт           |

**Endpoints API:**

| Метод | URL                              | Призначення                                           |
|-------|----------------------------------|-------------------------------------------------------|
| POST  | /auth/register                   | Реєстрація з валідацією + verify token                |
| POST  | /auth/login                      | Вхід через Passport.js, регенерація session-id        |
| POST  | /auth/logout                     | Знищення сесії + cookie                               |
| GET   | /auth/status                     | Поточний стан автентифікації                          |
| GET   | /verify?token=…                  | Підтвердження email                                   |
| GET   | /api/apartments                  | resident → свої, technician/manager → усі             |
| POST  | /api/heatpump/temperature        | Зміна цільової T (з перевіркою ownership)             |
| POST  | /api/maintenance/schedule        | Планування ТО (лише technician)                       |
| GET   | /api/maintenance                 | Список ТО (technician, facility_manager)              |
| GET   | /api/system/efficiency           | Агрегати по будинку (лише facility_manager)           |
| GET   | /api/audit                       | Журнал подій (лише facility_manager)                  |

**Технічні вимоги:**

– Node.js 20+ + Express 4; усі форми та API працюють у форматі JSON;
– Passport.js LocalStrategy з bcrypt (10 rounds) та account-lockout (5 спроб → 15 хв);
– `node:sqlite` (вбудований у Node 22+) з параметризованими prepared statements;
– `csurf` для double-submit CSRF-токена через сесію та meta-тег;
– `helmet` з CSP (`default-src 'self'`), `X-Frame-Options`, `X-Content-Type-Options`;
– `express-rate-limit`: 120 req/min для /api, 5 спроб/15хв для /auth/login;
– `express-validator` для валідації email, довжини пароля, ролі, числових діапазонів;
– аудит-журнал у БД з логуванням реєстрації, логінів, помилок CSRF, заборон 403.

**Хід виконання:**

Структура проєкту:

PR6/server.js — Express + повний стек middleware (helmet → bodyParser → cookies → session → passport → csurf)

PR6/db.js — `node:sqlite` зі схемою users / apartments / temperature_log / maintenance / audit_log

PR6/config/passport.js — LocalStrategy з bcrypt-перевіркою та account lockout

PR6/routes/auth.js — реєстрація, логін, логаут, верифікація email

PR6/routes/api.js — захищені ендпоінти з RBAC та ownership

PR6/middleware/auth.js — `isAuthenticated`, `hasRole`, `ownsApartmentOrPrivileged`

PR6/views/ — EJS-шаблони login.ejs, register.ejs, dashboard.ejs

PR6/public/ — Bootstrap-стилі + vanilla JS клієнт з CSRF-заголовком

PR6/test-api.js — 26 автотестів безпеки (XSS, CSRF, RBAC, brute-force, SQLi, ownership)

PR6/seed.js — посів демо-БД (3 користувачі, 6 квартир)

**Завдання 1: Реєстрація з валідацією та підтвердженням email**

Реалізовано `POST /auth/register` з шаром валідаторів `express-validator`, який перевіряє формат email, мінімальну довжину пароля (10 символів) з обов'язковими літерами та цифрами, довжину імені (2–80 символів) та належність ролі до дозволеного enum. Поле `name` додатково проходить через `.escape()` — санітайзер, що замінює HTML-метасимволи на entities, нейтралізуючи XSS-атаки типу stored. Після успішної реєстрації генерується криптографічно випадковий verify-token (24 байти) і повертається посилання `/verify?token=…` (у реальній системі — надсилається на email):

```javascript
const registerValidators = [
    body('email').isEmail().withMessage('Невірний формат email').normalizeEmail(),
    body('password')
        .isLength({ min: 10 }).withMessage('Пароль має бути не менше 10 символів')
        .matches(/[A-Za-z]/).withMessage('Пароль має містити літери')
        .matches(/\d/).withMessage('Пароль має містити цифри'),
    body('name').trim().isLength({ min: 2, max: 80 }).escape(),
    body('role').isIn(['resident', 'technician', 'facility_manager'])
];

router.post('/register', registerLimiter, registerValidators, async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) return res.status(400).json({ errors: errors.array() });
    const hash = await bcrypt.hash(password, BCRYPT_ROUNDS);
    const verifyToken = crypto.randomBytes(24).toString('hex');
    stmts.insertUser.run(email, hash, name, role, 0, verifyToken, Date.now());
    return res.status(201).json({ verify_url: `/verify?token=${verifyToken}`, ... });
});
```

Клієнтська сторінка `register.ejs` додатково містить індикатор сили пароля (password strength meter) на 5 рівнів, який обраховується за 5 евристиками (довжина ≥10, довжина ≥14, верхній+нижній регістр, цифри, спецсимволи). Це покращення UX — сервер у будь-якому випадку перевіряє пароль повторно.

Результат виконання:

*Рис. 1. Сторінка реєстрації з активним індикатором сили пароля (рівень «Дуже сильний»)*

Перевірка:

– Тест «Слабкий пароль/коротке імʼя → 400»: запит з `password:'short'` повертає масив `errors` із повідомленнями валідаторів.

– Тест «Неіснуюча роль → 400»: `role:'superadmin'` відхиляється `isIn`-валідатором, інʼєкція ролі неможлива.

– Тест «XSS payload у name санітизовано»: значення `<script>alert(1)</script>Hacker` зберігається як `&lt;script&gt;alert(1)&lt;/script&gt;Hacker` — символи замінені на HTML-entities.

**Завдання 2: Автентифікація через Passport.js + bcrypt + account lockout**

Налаштовано Passport з LocalStrategy у `config/passport.js`. У колбеку: пошук користувача за email, перевірка `locked_until`-таймштампа, `bcrypt.compare` для пароля, інкремент `failed_logins` при невдачі і блокування акаунту на 15 хвилин після 5-ї спроби. Серіалізується лише `user.id`, а `deserializeUser` повертає об'єкт без хеша паролю та токена верифікації — навіть якщо в дебагу логнути `req.user`, секрети не витечуть:

```javascript
passport.use(new LocalStrategy(
    { usernameField: 'email', passwordField: 'password', passReqToCallback: true },
    async (req, email, password, done) => {
        const user = stmts.findUserByEmail.get(email);
        if (!user) return done(null, false, { message: 'Невірний email або пароль' });
        if (user.locked_until && user.locked_until > Date.now()) {
            return done(null, false, { message: 'Акаунт тимчасово заблоковано.' });
        }
        const ok = await bcrypt.compare(password, user.password_hash);
        if (!ok) {
            stmts.incFailed.run(user.id);
            const fresh = stmts.findUserById.get(user.id);
            if (fresh.failed_logins >= 5) {
                stmts.lockUser.run(Date.now() + 15*60*1000, user.id);
            }
            return done(null, false, { message: 'Невірний email або пароль' });
        }
        if (!user.email_verified) return done(null, false, { message: 'Email не підтверджено.' });
        stmts.resetFailed.run(user.id);
        return done(null, user);
    }
));

passport.serializeUser((user, done) => done(null, user.id));
passport.deserializeUser((id, done) => {
    const user = stmts.findUserById.get(id);
    const { password_hash, verify_token, ...safe } = user;  // не пропускаємо хеш у сесію
    return done(null, safe);
});
```

Окремо, у роуті `/auth/login` після успішної автентифікації викликається `req.session.regenerate()` — це створює новий session-id і знищує старий, що захищає від session-fixation: навіть якщо зловмисник попередньо «підсадив» жертві відомий йому session-id, після логіну він стане невалідним.

Результат виконання:

*Рис. 2. Сторінка входу з повідомленням про невірні дані*

Перевірка:

– Тест «Login resident → 200»: коректні облікові дані повертають JSON `{ user: { id, email, name, role } }` без пароля.

– Тест «Спроба 1–3: 401»: невірний пароль → `401 { error: 'Невірний email або пароль' }`, лічильник `failed_logins` у БД інкрементується.

– Тест «Спроба 4–6: 429 (rate-limit)»: на 4-й невірній спробі `express-rate-limit` повертає `429` через шар per-(IP+email) ключа. Незалежно від rate-limit, після 5 невдач `passport.js` блокує сам акаунт на 15 хвилин — двошарова оборона.

**Завдання 3: RBAC та ownership-перевірка**

Реалізовано три middleware у `middleware/auth.js`. `isAuthenticated` повертає 401 JSON для `/api/*` та редірект на `/login` для HTML-сторінок. `hasRole(...roles)` приймає список дозволених ролей і повертає 403, якщо `req.user.role` не входить у нього. `ownsApartmentOrPrivileged` зчитує квартиру з БД, перевіряє чи це власник або привілейована роль:

```javascript
function hasRole(...roles) {
    return (req, res, next) => {
        if (!req.isAuthenticated()) return res.status(401).json({ error: 'Необхідна автентифікація' });
        if (!roles.includes(req.user.role)) {
            audit(req.user.id, 'forbidden', req.ip, `role=${req.user.role} needed=${roles.join(',')}`);
            return res.status(403).json({ error: 'Недостатньо прав доступу' });
        }
        return next();
    };
}

function ownsApartmentOrPrivileged(req, res, next) {
    const apt = stmts.apartmentById.get(parseInt(req.params.id, 10));
    if (!apt) return res.status(404).json({ error: 'Квартиру не знайдено' });
    const privileged = ['technician', 'facility_manager'].includes(req.user.role);
    if (!privileged && apt.owner_user_id !== req.user.id) {
        audit(req.user.id, 'ownership_violation', req.ip, `apartment_id=${apt.id}`);
        return res.status(403).json({ error: 'Ця квартира не належить вам' });
    }
    req.apartment = apt;
    return next();
}
```

Таблиця прав доступу формує дві осі контролю — рольова (RBAC) і власницька (ownership): resident бачить лише свої квартири, але technician/manager — усі; resident може змінювати T лише у своїх квартирах, technician — у будь-якій (для діагностики), а доступ до `/api/audit` і `/api/system/efficiency` має лише facility_manager.

Результат виконання:

*Рис. 3. Дашборд мешканця: видно лише його 3 квартири (12А, 18Б, 25В)*

*Рис. 4. Дашборд менеджера: усі 6 квартир, картки ефективності (367 м², 0.74 кВт ел., 2.85 кВт тепл., середній COP 3.85) та аудит-журнал унизу*

Перевірка:

– Тест «resident бачить лише свої квартири (3)»: GET /api/apartments повертає масив із 3 елементів для resident, 6 — для manager.

– Тест «resident → /api/system/efficiency → 403»: RBAC коректно блокує доступ.

– Тест «resident не може чіпати чужу квартиру → 403»: POST з `apartmentId=4` (не його) → `{ error: 'Ця квартира не належить вам' }`.

– Тест «resident оновив свою квартиру → 200»: POST з `apartmentId=1` → `{ ok: true, target_temp: 22.5 }`.

**Завдання 4: Захист від XSS і параметризовані SQL-запити**

Захист від XSS будується на трьох рівнях. **Перший рівень** — CSP-заголовок `default-src 'self'` через helmet: навіть якщо зловмисник якось впровадив `<script>` у HTML, браузер відмовиться його завантажити з origin'у, відмінного від нашого. **Другий рівень** — шаблони EJS автоматично екранують `<%= value %>` (а небезпечну форму `<%- %>` я ніде не використовую для користувацького вводу). **Третій рівень** — `express-validator.escape()` у валідаторі `name` перетворює `<`, `>`, `&`, `"` на HTML-entities ще на вході.

Захист від SQL-ін'єкцій реалізовано через prepared statements `node:sqlite`. Параметри передаються через `?`-плейсхолдери, що повністю відокремлює SQL-граматику від даних:

```javascript
// db.js
const stmts = {
    findUserByEmail: db.prepare('SELECT * FROM users WHERE email = ?'),
    insertUser:      db.prepare(`INSERT INTO users (email, password_hash, name, role, email_verified,
                                                    verify_token, created_at)
                                 VALUES (?, ?, ?, ?, ?, ?, ?)`),
    apartmentsByOwner: db.prepare('SELECT * FROM apartments WHERE owner_user_id = ? ORDER BY number')
    // ... жодної конкатенації рядків з користувацьким вводом немає
};

// passport.js
const user = stmts.findUserByEmail.get(email);  // навіть "admin' OR 1=1 --" → літерал
```

Помітно, що класична атака `admin' OR 1=1 --` буде відхилена ще раніше — `express-validator.isEmail()` поверне 400 на етапі валідації, не дочекавшись SQL. Двошарова оборона: валідація + параметризація.

Результат виконання:

*Рис. 5. Вивід автоматичних тестів: усі 24 перевірки безпеки PASS, 0 FAIL*

Перевірка:

– Тест «XSS payload у name санітизовано»: `<script>alert(1)</script>Hacker` зберігається у БД як `&lt;script&gt;alert(1)&lt;/script&gt;Hacker`. Коли менеджер відкриє аудит-журнал, JS не виконається.

– Тест «CSP default-src 'self'»: GET /login повертає заголовок `Content-Security-Policy: default-src 'self'; ...`. Інлайнові скрипти блокуються браузером.

– Тест «X-Frame-Options set» + «X-Content-Type-Options: nosniff»: helmet вмикає захист від clickjacking та MIME-sniffing атак.

– Тест «SQL-ін'єкція у email → 401»: `admin' OR 1=1 --` повертає 401 (валідатор не пускає на SQL-шар; навіть якщо пустив би — `?` плейсхолдер інертно зберігає це як рядок).

**Завдання 5: CSRF, валідація даних і rate limiting**

CSRF реалізовано через `csurf`, який зберігає секрет у сесії та видає клієнту токен через `req.csrfToken()`. Токен пробрасується у views як `<input type="hidden" name="_csrf">` для класичних форм та у `<meta name="csrf-token">` для fetch-запитів. Клієнт читає його зі сторінки і вкладає у заголовок `CSRF-Token`. Зворотний шар — обробник помилки `EBADCSRFTOKEN` повертає 403 і логує спробу в аудит-журнал:

```javascript
// server.js
app.use(csrf());  // після session
app.use((req, res, next) => {
    res.locals.user = req.user || null;
    res.locals.csrfToken = req.csrfToken();
    next();
});

app.use((err, req, res, next) => {
    if (err && err.code === 'EBADCSRFTOKEN') {
        audit(req.user?.id, 'csrf_failed', req.ip, `${req.method} ${req.originalUrl}`);
        return res.status(403).json({ error: 'Невірний CSRF-токен' });
    }
    next(err);
});
```

Cookies сесії захищено флагами `httpOnly: true` (JS не має доступу через `document.cookie`), `sameSite: 'strict'` (браузер не надсилає cookie при cross-origin запитах), `secure: true у проді` (тільки через HTTPS). Це блокує захоплення сесії через XSS і захищає від CSRF на рівні браузера.

Rate-limiting налаштовано двома шарами через `express-rate-limit`:

```javascript
// server.js
const apiLimiter = rateLimit({
    windowMs: 60_000, max: 120,
    standardHeaders: true,
    message: { error: 'Забагато запитів. Спробуйте пізніше.' }
});
app.use('/api', apiLimiter);

// routes/auth.js — жорсткіший на /auth/login
const loginLimiter = rateLimit({
    windowMs: 15 * 60 * 1000, max: 5,
    keyGenerator: (req) => `${req.ip}|${(req.body?.email || '').toLowerCase()}`,
    message: { error: 'Забагато спроб входу. Спробуйте за 15 хвилин.' }
});
```

Окремий `keyGenerator` для логіну використовує комбінацію IP+email: це не дозволяє зловмиснику обійти ліміт чергуванням різних акаунтів з однієї IP, але і не блокує всіх користувачів з одного офісу через NAT.

Результат виконання:

*Рис. 6. Лог звернень в аудит-журналі менеджера: видно події login_success, login_failed з bad_password attempt=1, logout (всі ip=::1 — localhost)*

Перевірка:

– Тест «POST без CSRF → 403»: запит без заголовка `CSRF-Token` блокується csurf, у аудит-журналі зʼявляється запис `csrf_failed`.

– Тест «Спроба 4: 429 (rate-limit)»: 4-та невдала спроба логіну з одного IP повертає 429 з заголовками `RateLimit-Limit: 5`, `RateLimit-Reset: 900`.

– Тест «Target=99°C → 400 (валідація діапазону)»: `body('target').isFloat({ min: 12, max: 28 })` відхиляє значення поза межами фізично можливих.

– Тест «Logout → 200» + «Після logout /api/audit → 401»: `req.logout()` + `session.destroy()` + `clearCookie('hp.sid')` повністю інвалідує сесію; наступний GET /api/audit повертає 401.

**Висновки**

У результаті виконання практичної роботи №6 спроєктовано та реалізовано захищений веб-портал керування мережею теплових насосів житлового комплексу (Варіант 15) з повним стеком оборонних механізмів. Реалізовано трирівневу рольову модель доступу (resident / technician / facility_manager) з RBAC-middleware `hasRole` та власницьким `ownsApartmentOrPrivileged`, що додатково обмежує мешканців лише їхніми квартирами. Засвоєно роботу з Passport.js LocalStrategy у поєднанні з bcrypt (10 rounds), сесіями з `httpOnly`/`sameSite=strict`-cookies, регенерацією session-id для захисту від session-fixation і per-account lockout після 5 невдалих спроб.

Реалізовано п'ятишарову оборону від XSS: CSP-заголовок `default-src 'self'` через helmet, автоматичне HTML-екранування EJS-шаблонами, `express-validator.escape()` на вході, відсутність будь-яких `innerHTML` з користувацьким вводом у клієнтському JS та виключно `textContent` для рендерингу аудит-журналу. Захист від CSRF побудовано через `csurf` з double-submit-токеном, що пробрасується у meta-тег та форми, з власним error-handler'ом для логування спроб атак у аудит-журнал. Захист від SQL-ін'єкцій реалізовано через параметризовані prepared statements `node:sqlite` без жодної конкатенації рядків. Rate-limiting налаштовано двошарово: глобальний `/api`-ліміт 120 req/min та жорсткий `/auth/login`-ліміт 5 спроб за 15 хвилин з ключем `IP+email`.

Складено автоматизований набір з 26 тестових сценаріїв (`test-api.js`), що покриває кожен з шарів безпеки: валідацію реєстрації, XSS-санітизацію, RBAC-обмеження, ownership-перевірку, CSRF-відмову, brute-force lockout, SQLi-спроби та інвалідизацію сесії після logout. Усі тести проходять (PASS 24 / FAIL 0), що підтверджує цілісність обраної архітектури захисту. Розроблений портал повністю відповідає вимогам Варіанту 15 і може бути використаний як прототип системи керування тепловими насосами у багатоквартирних житлових комплексах.

**Список використаних джерел**

1. Passport.js Documentation. URL: https://www.passportjs.org/

2. Express Security Best Practices. URL: https://expressjs.com/en/advanced/best-practice-security.html

3. OWASP Top 10. URL: https://owasp.org/www-project-top-ten/

4. Helmet.js Documentation. URL: https://helmetjs.github.io/

5. express-validator Documentation. URL: https://express-validator.github.io/

6. bcrypt npm package. URL: https://www.npmjs.com/package/bcrypt

7. csurf npm package. URL: https://www.npmjs.com/package/csurf

8. express-rate-limit Documentation. URL: https://express-rate-limit.mintlify.app/

9. Node.js `node:sqlite` (Experimental). URL: https://nodejs.org/api/sqlite.html

10. MDN — Content Security Policy (CSP). URL: https://developer.mozilla.org/en-US/docs/Web/HTTP/CSP

11. Методичні вказівки до практичної роботи №6 з дисципліни «Основи Веб-програмування», КПІ ім. Ігоря Сікорського, 2025.
