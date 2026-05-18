// Login form: CSRF-token у заголовку, fetch JSON.
const csrf = document.querySelector('meta[name="csrf-token"]').content;
const form = document.getElementById('login-form');
const errBox = document.getElementById('js-error');

form.addEventListener('submit', async (e) => {
    e.preventDefault();
    errBox.classList.add('d-none');
    const fd = new FormData(form);
    const payload = { email: fd.get('email'), password: fd.get('password') };
    const res = await fetch('/auth/login', {
        method: 'POST',
        credentials: 'same-origin',
        headers: {
            'Content-Type': 'application/json',
            'CSRF-Token': csrf
        },
        body: JSON.stringify(payload)
    });
    const data = await res.json().catch(() => ({}));
    if (res.ok) {
        location.href = '/dashboard';
    } else {
        errBox.textContent = data.error || data.errors?.[0]?.msg || 'Помилка входу';
        errBox.classList.remove('d-none');
    }
});
