// Реєстрація: CSRF + клієнтський індикатор сили пароля + fetch.
const csrf = document.querySelector('meta[name="csrf-token"]').content;
const form = document.getElementById('register-form');
const errBox = document.getElementById('js-error');
const okBox  = document.getElementById('js-success');
const pwd    = document.getElementById('pwd');
const bar    = document.getElementById('pwd-bar');
const label  = document.getElementById('pwd-label');

function score(p) {
    let s = 0;
    if (p.length >= 10) s++;
    if (p.length >= 14) s++;
    if (/[a-z]/.test(p) && /[A-Z]/.test(p)) s++;
    if (/\d/.test(p)) s++;
    if (/[^A-Za-z0-9]/.test(p)) s++;
    return s; // 0..5
}

pwd.addEventListener('input', () => {
    const s = score(pwd.value);
    const pct = (s / 5) * 100;
    bar.style.width = pct + '%';
    bar.className = 'progress-bar ' + (s <= 2 ? 'bg-danger' : s === 3 ? 'bg-warning' : 'bg-success');
    label.textContent = ['Дуже слабкий', 'Слабкий', 'Середній', 'Прийнятний', 'Сильний', 'Дуже сильний'][s];
});

form.addEventListener('submit', async (e) => {
    e.preventDefault();
    errBox.classList.add('d-none');
    okBox.classList.add('d-none');
    const fd = new FormData(form);
    const payload = {
        email:    fd.get('email'),
        password: fd.get('password'),
        name:     fd.get('name'),
        role:     fd.get('role')
    };
    const res = await fetch('/auth/register', {
        method: 'POST',
        credentials: 'same-origin',
        headers: { 'Content-Type': 'application/json', 'CSRF-Token': csrf },
        body: JSON.stringify(payload)
    });
    const data = await res.json().catch(() => ({}));
    if (res.status === 201) {
        okBox.innerHTML = `Акаунт створено. Підтвердіть email: <a href="${data.verify_url}" class="alert-link">${data.verify_url}</a>`;
        okBox.classList.remove('d-none');
        form.reset();
        bar.style.width = '0%';
        label.textContent = 'Введіть пароль…';
    } else {
        const msg = data.errors ? data.errors.map(e => e.msg).join('; ') : (data.error || 'Помилка');
        errBox.textContent = msg;
        errBox.classList.remove('d-none');
    }
});
