// Застосунок ЛР4: та сама сторінка «Hello, DevOps World!», але як HTTP-сервіс на Node.js,
// щоб було що тестувати й на чому показати перевірки готовності та життєздатності (лекція 4).
import os from 'node:os';

const TEXT = 'text/plain; charset=utf-8';

function page({ version, env, pod }) {
  return `<!DOCTYPE html>
<html lang="uk">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Hello, DevOps World!</title>
    <style>
      body { font-family: system-ui, sans-serif; margin: 0; min-height: 100vh;
             display: grid; place-items: center; background: #f4f7f9; color: #1d2b36; }
      main { text-align: center; padding: 2rem; }
      h1   { font-size: 2.4rem; margin: 0 0 1rem; color: #0b4f6c; }
      p    { margin: .35rem 0; line-height: 1.4; }
      code { background: #e3ebf0; padding: .1rem .35rem; border-radius: 4px; }
    </style>
  </head>
  <body>
    <main>
      <h1>Hello, DevOps!</h1>
      <p>ЛР4: CI/CD і Kubernetes. Степаненко Назар, ТВ-43, № 14.</p>
      <p>Версія: <code>${version}</code> · середовище: <code>${env}</code></p>
      <p>Відповів под: <code>${pod}</code></p>
    </main>
  </body>
</html>
`;
}

export function createApp(options = {}) {
  const config = {
    version: options.version || 'dev',
    env: options.env || 'local',
    pod: options.pod || os.hostname(),
    warmupMs: options.warmupMs ?? 0,
    dependencyUrl: options.dependencyUrl || '',
    dependencyTimeoutMs: options.dependencyTimeoutMs ?? 500,
  };
  const now = options.now ?? Date.now;
  const fetchFn = options.fetch ?? globalThis.fetch;
  const startedAt = now();
  let draining = false;

  const warmedUp = () => now() - startedAt >= config.warmupMs;

  async function dependencyOk() {
    if (!config.dependencyUrl) return true;
    try {
      const res = await fetchFn(config.dependencyUrl, {
        signal: AbortSignal.timeout(config.dependencyTimeoutMs),
      });
      return res.ok;
    } catch {
      return false;
    }
  }

  // Готовність: чи можна слати на цей екземпляр трафік. Перевіряє все, від чого залежить
  // обслуговування запитів, зокрема зовнішню залежність.
  async function readiness() {
    if (draining) return { ready: false, reason: 'draining' };
    if (!warmedUp()) return { ready: false, reason: 'warming up' };
    if (!(await dependencyOk())) return { ready: false, reason: 'dependency unavailable' };
    return { ready: true, reason: 'ok' };
  }

  function send(res, status, type, body) {
    res.writeHead(status, {
      'content-type': type,
      'cache-control': 'no-store',
      'x-app-version': config.version,
      'x-pod': config.pod,
    });
    res.end(body);
  }

  async function handler(req, res) {
    const { pathname } = new URL(req.url, 'http://localhost');
    if (req.method !== 'GET' && req.method !== 'HEAD') {
      return send(res, 405, TEXT, 'method not allowed\n');
    }
    // Життєздатність: лише «процес живий і відповідає», без зовнішніх залежностей.
    if (pathname === '/healthz') return send(res, 200, TEXT, 'ok\n');
    if (pathname === '/readyz') {
      const r = await readiness();
      return send(res, r.ready ? 200 : 503, TEXT, `${r.reason}\n`);
    }
    if (pathname !== '/' && pathname !== '/api/info') return send(res, 404, TEXT, 'not found\n');
    // Поки застосунок «прогрівається» (у реальному сервісі - з'єднання з БД, кеш),
    // запити користувачів він обслужити не може.
    if (!warmedUp()) return send(res, 503, TEXT, 'warming up\n');
    if (pathname === '/api/info') {
      const info = {
        version: config.version,
        env: config.env,
        pod: config.pod,
        uptimeSeconds: Math.round((now() - startedAt) / 1000),
      };
      return send(res, 200, 'application/json', `${JSON.stringify(info)}\n`);
    }
    return send(res, 200, 'text/html; charset=utf-8', page(config));
  }

  return {
    handler,
    readiness,
    config,
    startDraining() {
      draining = true;
    },
  };
}
