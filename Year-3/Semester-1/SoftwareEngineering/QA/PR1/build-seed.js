// Usage: node build-seed.js [artem@email]  ->  writes seed.run.js with backlog data inlined
const fs = require('fs');
const path = require('path');

const dir = __dirname;
const data = fs.readFileSync(path.join(dir, 'backlog.json'), 'utf8');
const email = process.argv[2] ? JSON.stringify(process.argv[2]) : 'null';
const code = fs.readFileSync(path.join(dir, 'jira-seed.js'), 'utf8')
  .replace('__DATA__', () => data.trim())
  .replace('__ARTEM_EMAIL__', () => email);
fs.writeFileSync(path.join(dir, 'seed.run.js'), code);
console.log('seed.run.js written');
