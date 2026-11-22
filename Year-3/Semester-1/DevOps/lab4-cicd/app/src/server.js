import http from 'node:http';
import { createApp } from './app.js';

const app = createApp({
  version: process.env.APP_VERSION,
  env: process.env.APP_ENV,
  warmupMs: Number(process.env.WARMUP_MS ?? 0),
  dependencyUrl: process.env.DEPENDENCY_URL,
});
const port = Number(process.env.PORT ?? 8080);
const drainMs = Number(process.env.DRAIN_MS ?? 0);

const server = http.createServer((req, res) => {
  app.handler(req, res).catch((err) => {
    console.error(err);
    if (!res.headersSent) res.writeHead(500);
    res.end();
  });
});

server.listen(port, () => {
  const { version, env, warmupMs, dependencyUrl } = app.config;
  console.log(JSON.stringify({ msg: 'listening', port, version, env, warmupMs, dependencyUrl }));
});

// Плавне завершення: після SIGTERM екземпляр перестає бути готовим, але ще drainMs
// обслуговує запити, поки балансувальник прибирає його зі списку одержувачів.
function shutdown(signal) {
  console.log(JSON.stringify({ msg: 'shutdown', signal, drainMs }));
  app.startDraining();
  setTimeout(() => {
    server.close(() => process.exit(0));
    server.closeIdleConnections();
    setTimeout(() => process.exit(0), 5000).unref();
  }, drainMs);
}
process.once('SIGTERM', shutdown);
process.once('SIGINT', shutdown);
