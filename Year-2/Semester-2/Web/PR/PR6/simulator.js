// Симуляція теплових насосів: тік раз на 5с, наближає current_temp до target_temp.
// Спрощена теплова модель: ΔT_per_tick = (target - current) × k, де k залежить від COP.
const { stmts } = require('./db');

const TICK_MS = 5000;
const K_BASE = 0.04; // швидкість зближення (наближено)

function tick() {
    const apartments = stmts.listApartments.all();
    for (const a of apartments) {
        const diff = a.target_temp - a.current_temp;
        // ΔT за тік + малий дрейф ±0.05°C
        const drift = (Math.random() - 0.5) * 0.1;
        const newCurrent = a.current_temp + diff * K_BASE + drift;
        // Поточна теплова потужність (кВт): пропорційна |ΔT| × площі / COP
        const heatNeed = Math.abs(diff) * a.area_m2 * 0.03;
        const electrical = +(heatNeed / a.cop).toFixed(2);
        stmts.updateApartmentCurrent.run(+newCurrent.toFixed(2), electrical, a.id);
    }
}

function start() {
    tick();
    return setInterval(tick, TICK_MS);
}

module.exports = { start, TICK_MS };
