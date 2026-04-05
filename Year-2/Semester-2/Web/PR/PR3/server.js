/**
 * Реєстр резервних джерел живлення (ДГУ)
 * Практична робота №3 · HTML-форми та серверна обробка (Node.js + Express)
 * Варіант 15
 */

const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

// Middleware
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'));

// Шлях до файлу даних
const DATA_FILE = path.join(__dirname, 'data', 'generators.json');

// ==================== Допоміжні функції ====================

/**
 * Читає дані з JSON-файлу.
 * @returns {Array} масив записів ДГУ
 */
function readData() {
    try {
        if (!fs.existsSync(DATA_FILE)) {
            return [];
        }
        const data = fs.readFileSync(DATA_FILE, 'utf8');
        return JSON.parse(data);
    } catch (error) {
        console.error('Помилка читання даних:', error);
        return [];
    }
}

/**
 * Записує дані у JSON-файл.
 * @param {Array} data - масив записів для збереження
 * @returns {boolean} true при успіху
 */
function writeData(data) {
    try {
        const dir = path.dirname(DATA_FILE);
        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }
        fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2), 'utf8');
        return true;
    } catch (error) {
        console.error('Помилка запису даних:', error);
        return false;
    }
}

/**
 * Валідація даних ДГУ на сервері.
 * @param {Object} body - тіло запиту
 * @returns {Object} { valid: boolean, errors: string[] }
 */
function validateGenerator(body) {
    const errors = [];

    if (!body.objectName || body.objectName.trim().length < 2) {
        errors.push("Назва об'єкта має містити щонайменше 2 символи");
    }
    if (!body.generatorType) {
        errors.push('Оберіть тип ДГУ');
    }
    const power = parseFloat(body.power);
    if (isNaN(power) || power <= 0 || power > 50000) {
        errors.push('Потужність має бути від 0.1 до 50 000 кВА');
    }
    if (!body.fuelType) {
        errors.push('Оберіть тип палива');
    }
    if (!body.address || body.address.trim().length < 5) {
        errors.push('Адреса має містити щонайменше 5 символів');
    }
    const consumption = parseFloat(body.fuelConsumption);
    if (isNaN(consumption) || consumption <= 0) {
        errors.push('Витрата палива має бути більше 0');
    }
    if (!body.status) {
        errors.push('Оберіть стан ДГУ');
    }

    return { valid: errors.length === 0, errors };
}

// ==================== API маршрути ====================

/**
 * GET /api/generators — отримати всі записи з підтримкою фільтрації та пошуку.
 * Query параметри: search, type, status, fuelType
 */
app.get('/api/generators', (req, res) => {
    let generators = readData();
    const { search, type, status, fuelType } = req.query;

    // Пошук за назвою або адресою
    if (search) {
        const q = search.toLowerCase();
        generators = generators.filter(g =>
            g.objectName.toLowerCase().includes(q) ||
            g.address.toLowerCase().includes(q)
        );
    }

    // Фільтрація за типом ДГУ
    if (type) {
        generators = generators.filter(g => g.generatorType === type);
    }

    // Фільтрація за станом
    if (status) {
        generators = generators.filter(g => g.status === status);
    }

    // Фільтрація за типом палива
    if (fuelType) {
        generators = generators.filter(g => g.fuelType === fuelType);
    }

    res.json(generators);
});

/**
 * GET /api/generators/stats — статистика по реєстру.
 */
app.get('/api/generators/stats', (req, res) => {
    const generators = readData();

    const totalPower = generators.reduce((sum, g) => sum + parseFloat(g.power), 0);
    const avgConsumption = generators.length > 0
        ? generators.reduce((sum, g) => sum + parseFloat(g.fuelConsumption), 0) / generators.length
        : 0;

    const byType = {};
    const byStatus = {};
    const byFuel = {};

    generators.forEach(g => {
        byType[g.generatorType] = (byType[g.generatorType] || 0) + 1;
        byStatus[g.status] = (byStatus[g.status] || 0) + 1;
        byFuel[g.fuelType] = (byFuel[g.fuelType] || 0) + 1;
    });

    res.json({
        total: generators.length,
        totalPower: Math.round(totalPower * 10) / 10,
        avgConsumption: Math.round(avgConsumption * 10) / 10,
        byType,
        byStatus,
        byFuel
    });
});

/**
 * POST /api/generators — додати новий запис ДГУ.
 */
app.post('/api/generators', (req, res) => {
    const validation = validateGenerator(req.body);
    if (!validation.valid) {
        return res.status(400).json({
            success: false,
            message: 'Помилка валідації',
            errors: validation.errors
        });
    }

    try {
        const newGenerator = {
            id: Date.now().toString(),
            objectName: req.body.objectName.trim(),
            generatorType: req.body.generatorType,
            power: parseFloat(req.body.power),
            fuelType: req.body.fuelType,
            address: req.body.address.trim(),
            fuelConsumption: parseFloat(req.body.fuelConsumption),
            lastMaintenance: req.body.lastMaintenance || null,
            nextMaintenance: req.body.nextMaintenance || null,
            status: req.body.status,
            notes: req.body.notes ? req.body.notes.trim() : '',
            registrationDate: new Date().toISOString()
        };

        const generators = readData();
        generators.push(newGenerator);

        if (writeData(generators)) {
            res.status(201).json({
                success: true,
                message: 'ДГУ успішно зареєстровано',
                data: newGenerator
            });
        } else {
            throw new Error('Помилка запису даних');
        }
    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Помилка реєстрації ДГУ',
            error: error.message
        });
    }
});

/**
 * PUT /api/generators/:id — оновити запис ДГУ.
 */
app.put('/api/generators/:id', (req, res) => {
    const validation = validateGenerator(req.body);
    if (!validation.valid) {
        return res.status(400).json({
            success: false,
            message: 'Помилка валідації',
            errors: validation.errors
        });
    }

    try {
        const generators = readData();
        const index = generators.findIndex(g => g.id === req.params.id);

        if (index === -1) {
            return res.status(404).json({
                success: false,
                message: 'ДГУ не знайдено'
            });
        }

        generators[index] = {
            ...generators[index],
            objectName: req.body.objectName.trim(),
            generatorType: req.body.generatorType,
            power: parseFloat(req.body.power),
            fuelType: req.body.fuelType,
            address: req.body.address.trim(),
            fuelConsumption: parseFloat(req.body.fuelConsumption),
            lastMaintenance: req.body.lastMaintenance || null,
            nextMaintenance: req.body.nextMaintenance || null,
            status: req.body.status,
            notes: req.body.notes ? req.body.notes.trim() : ''
        };

        if (writeData(generators)) {
            res.json({
                success: true,
                message: 'ДГУ успішно оновлено',
                data: generators[index]
            });
        } else {
            throw new Error('Помилка запису даних');
        }
    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Помилка оновлення ДГУ'
        });
    }
});

/**
 * DELETE /api/generators/:id — видалити запис ДГУ.
 */
app.delete('/api/generators/:id', (req, res) => {
    try {
        const generators = readData();
        const filtered = generators.filter(g => g.id !== req.params.id);

        if (filtered.length === generators.length) {
            return res.status(404).json({
                success: false,
                message: 'ДГУ не знайдено'
            });
        }

        if (writeData(filtered)) {
            res.json({
                success: true,
                message: 'ДГУ видалено з реєстру'
            });
        } else {
            throw new Error('Помилка запису даних');
        }
    } catch (error) {
        res.status(500).json({
            success: false,
            message: 'Помилка видалення ДГУ'
        });
    }
});

// Запуск сервера
app.listen(PORT, () => {
    console.log(`Сервер запущено: http://localhost:${PORT}`);
});
