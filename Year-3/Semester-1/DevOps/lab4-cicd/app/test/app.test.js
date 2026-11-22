import { test } from 'node:test';
import assert from 'node:assert/strict';
import http from 'node:http';
import { createApp } from '../src/app.js';

// Піднімає застосунок на випадковому порту і повертає адресу.
async function start(t, options = {}) {
  const app = createApp({ pod: 'test-pod', ...options });
  const server = http.createServer((req, res) => app.handler(req, res));
  await new Promise((resolve) => server.listen(0, '127.0.0.1', resolve));
  t.after(() => new Promise((resolve) => server.close(resolve)));
  return { app, base: `http://127.0.0.1:${server.address().port}` };
}

// Керований годинник замість реального часу: тест не чекає секунди прогріву.
function fakeClock(start = 0) {
  let t = start;
  return { now: () => t, advance: (ms) => (t += ms) };
}

test('головна сторінка містить привітання, версію й под', async (t) => {
  const { base } = await start(t, { version: '1.2.3', env: 'ci' });
  const res = await fetch(`${base}/`);
  assert.equal(res.status, 200);
  assert.match(res.headers.get('content-type'), /text\/html/);
  assert.equal(res.headers.get('x-app-version'), '1.2.3');
  assert.equal(res.headers.get('x-pod'), 'test-pod');
  const html = await res.text();
  assert.match(html, /Hello, DevOps World!/);
  assert.match(html, /1\.2\.3/);
  assert.match(html, /test-pod/);
});

test('/api/info повертає JSON з версією й середовищем', async (t) => {
  const { base } = await start(t, { version: '1.2.3', env: 'ci' });
  const res = await fetch(`${base}/api/info`);
  assert.equal(res.status, 200);
  const info = await res.json();
  assert.deepEqual(
    { version: info.version, env: info.env, pod: info.pod },
    { version: '1.2.3', env: 'ci', pod: 'test-pod' },
  );
  assert.equal(typeof info.uptimeSeconds, 'number');
});

test('без явних параметрів версія dev, середовище local', async (t) => {
  const { base } = await start(t);
  const info = await (await fetch(`${base}/api/info`)).json();
  assert.equal(info.version, 'dev');
  assert.equal(info.env, 'local');
});

test('/healthz і /readyz відповідають 200 у звичайному стані', async (t) => {
  const { base } = await start(t);
  assert.equal((await fetch(`${base}/healthz`)).status, 200);
  const ready = await fetch(`${base}/readyz`);
  assert.equal(ready.status, 200);
  assert.equal(await ready.text(), 'ok\n');
});

test('під час прогріву застосунок живий, але не готовий', async (t) => {
  const clock = fakeClock();
  const { base } = await start(t, { warmupMs: 4000, now: clock.now });
  assert.equal((await fetch(`${base}/healthz`)).status, 200);
  const ready = await fetch(`${base}/readyz`);
  assert.equal(ready.status, 503);
  assert.equal(await ready.text(), 'warming up\n');
  assert.equal((await fetch(`${base}/`)).status, 503);
  assert.equal((await fetch(`${base}/api/info`)).status, 503);

  clock.advance(4000);
  assert.equal((await fetch(`${base}/readyz`)).status, 200);
  assert.equal((await fetch(`${base}/`)).status, 200);
});

test('недоступна залежність: не готовий, але живий (лекція 4)', async (t) => {
  const down = async () => {
    throw new Error('ECONNREFUSED');
  };
  const { base } = await start(t, { dependencyUrl: 'http://backend/healthz', fetch: down });
  const ready = await fetch(`${base}/readyz`);
  assert.equal(ready.status, 503);
  assert.equal(await ready.text(), 'dependency unavailable\n');
  // Життєздатність від залежності не залежить, тож перезапуску не буде.
  assert.equal((await fetch(`${base}/healthz`)).status, 200);
});

test('залежність відповідає помилкою або успіхом', async (t) => {
  let status = 500;
  const stub = async () => ({ ok: status < 400 });
  const { base } = await start(t, { dependencyUrl: 'http://backend/healthz', fetch: stub });
  assert.equal((await fetch(`${base}/readyz`)).status, 503);
  status = 200;
  assert.equal((await fetch(`${base}/readyz`)).status, 200);
});

test('після SIGTERM (draining) не готовий, але ще обслуговує запити', async (t) => {
  const { app, base } = await start(t);
  app.startDraining();
  const ready = await fetch(`${base}/readyz`);
  assert.equal(ready.status, 503);
  assert.equal(await ready.text(), 'draining\n');
  assert.equal((await fetch(`${base}/`)).status, 200);
});

test('невідомий шлях - 404, метод POST - 405, HEAD без тіла', async (t) => {
  const { base } = await start(t);
  assert.equal((await fetch(`${base}/nope`)).status, 404);
  assert.equal((await fetch(`${base}/`, { method: 'POST' })).status, 405);
  const head = await fetch(`${base}/`, { method: 'HEAD' });
  assert.equal(head.status, 200);
  assert.equal(await head.text(), '');
});
