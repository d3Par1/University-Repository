# Практична робота №4 — REST API для енергетичних даних (Node.js + Express)

## Розумна система вуличного освітлення

**Курс:** Основи Веб-програмування
**Варіант:** 15 — Розумна система вуличного освітлення
**Група:** ТВ-43
**Семестр:** 2-й, 2025/26 н.р.

## Опис

REST API для інтелектуального управління вуличним освітленням з автоматичним та ручним керуванням яскравістю, обліком енергоспоживання та віддаленим керуванням світильниками. Реалізовано на Node.js + Express; дані зберігаються у JSON-файлі. До API доданий веб-інтерфейс (Bootstrap 5) для інтерактивного тестування.

## Структура проєкту

```
PR4/
├── server.js                       — Express-сервер з REST API
├── test-api.js                     — автоматичні тести через fetch
├── package.json
├── public/
│   ├── index.html                  — інтерактивний симулятор API
│   ├── styles.css
│   └── script.js
├── data/
│   └── lighting-systems.json       — сховище даних
├── screenshots/
└── README.md
```

## Параметри об'єкта

| Параметр       | Тип     | Опис                                   |
|----------------|---------|----------------------------------------|
| id             | number  | Унікальний ідентифікатор системи       |
| name           | string  | Назва системи / сегменту               |
| lampCount      | number  | Загальна кількість світильників        |
| activeLamps    | number  | Кількість увімкнених                   |
| totalPower     | number  | Загальна споживана потужність (кВт)    |
| brightness     | number  | Рівень яскравості (%)                  |
| mode           | string  | `auto` / `manual` / `schedule`         |
| energyConsumed | number  | Спожита енергія (кВт·год)              |
| lightLevel     | number  | Виміряний рівень освітленості (люкс)   |
| schedule       | object  | `{ on: "HH:MM", off: "HH:MM" }`        |

## Endpoints API

| Метод  | URL                                          | Призначення                              |
|--------|----------------------------------------------|------------------------------------------|
| GET    | `/api/lighting-systems`                      | Список (фільтри, пошук, сортування, пагінація) |
| GET    | `/api/lighting-systems/stats`                | Статистика по всім системам              |
| GET    | `/api/lighting-systems/:id`                  | Деталі однієї системи                    |
| GET    | `/api/lighting-systems/:id/schedule`         | Розклад роботи системи                   |
| POST   | `/api/lighting-systems`                      | Створити нову систему                    |
| POST   | `/api/lighting-systems/:id/control`          | Керування (увімкнути/вимкнути світильники) |
| PUT    | `/api/lighting-systems/:id`                  | Повне оновлення                          |
| PATCH  | `/api/lighting-systems/:id`                  | Часткове оновлення                       |
| PUT    | `/api/lighting-systems/:id/brightness`       | Швидка зміна яскравості                  |
| DELETE | `/api/lighting-systems/:id`                  | Видалення                                |

## Query parameters для GET `/api/lighting-systems`

- `mode=auto|manual|schedule` — фільтр за режимом
- `search=...` — пошук за назвою (case-insensitive)
- `sort=brightness&order=desc` — сортування
- `minBrightness=50&maxBrightness=100` — діапазон яскравості
- `page=1&limit=10` — пагінація (повертає `{total, page, limit, data}`)

## Запуск

```bash
npm install
npm start          # сервер на http://localhost:3000
npm test           # запустити автотести (потрібен запущений сервер)
```

Веб-інтерфейс: http://localhost:3000/

## Технології

- **Сервер:** Node.js + Express 4
- **Сховище:** JSON-файл (`fs` module)
- **Клієнт:** HTML5, CSS3, Bootstrap 5.3, vanilla JS (Fetch API)
- **Валідація:** серверна перевірка типів, діапазонів та крос-полів (`activeLamps ≤ lampCount`)
- **Middleware:** `express.json`, `express.static`, кастомні логування + CORS
