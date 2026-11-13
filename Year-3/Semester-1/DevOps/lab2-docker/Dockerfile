# ---------- Етап 1: збірка сайту (Node.js потрібен лише тут) ----------
FROM node:24.22.2-alpine3.24 AS build
WORKDIR /app

# Спершу лише маніфести залежностей: поки вони не змінились,
# шар з npm ci береться з кешу, навіть якщо змінено сторінки.
COPY package.json package-lock.json ./
# Дзеркало npm можна передати під час збірки, не змінюючи Dockerfile
ARG NPM_REGISTRY=https://registry.npmjs.org/
RUN npm ci --registry="$NPM_REGISTRY" --no-audit --no-fund

# Потім вихідні файли, що змінюються найчастіше
COPY .htmlvalidate.json ./
COPY src/ src/
ARG APP_VERSION=dev
# Перевірка HTML (збірка падає на помилці), мінімізація, підстановка версії
RUN npm run build \
 && sed -i "s/__APP_VERSION__/${APP_VERSION}/g" dist/index.html

# ---------- Етап 2: образ, що запускається ----------
FROM nginx:1.30.6-alpine
ARG APP_VERSION=dev

LABEL org.opencontainers.image.title="hello-devops" \
      org.opencontainers.image.description="Hello, DevOps World! static page (DevOps lab 2)" \
      org.opencontainers.image.version="${APP_VERSION}" \
      org.opencontainers.image.authors="Nazar Stepanenko <Gangsterstepa@gmail.com>" \
      org.opencontainers.image.source="https://github.com/d3Par1/devops-lab1"

# Конфігурація середовища за замовчуванням; перевизначається під час запуску (-e)
ENV APP_ENV=production \
    APP_VERSION=${APP_VERSION} \
    NGINX_ENVSUBST_FILTER=^APP_

# Оновлення безпеки пакетів Alpine, що вийшли після збірки базового образу (Trivy).
# Підготовка до роботи без root: pid-файл у /tmp, кеш і conf.d належать nginx,
# директива user не потрібна, два воркери замість одного на кожне ядро хоста.
RUN apk upgrade --no-cache \
 && sed -i -e 's,^pid .*,pid /tmp/nginx.pid;,' \
           -e '/^user /d' \
           -e 's/^worker_processes .*/worker_processes 2;/' /etc/nginx/nginx.conf \
 && rm /etc/nginx/conf.d/default.conf \
 && chown -R nginx:nginx /var/cache/nginx /etc/nginx/conf.d

COPY nginx/default.conf.template /etc/nginx/templates/default.conf.template
COPY --from=build /app/dist/ /usr/share/nginx/html/

# Числовий UID (101 = nginx): так його може перевірити й оркестратор (runAsNonRoot)
USER 101:101
EXPOSE 80

HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
  CMD ["wget", "-q", "-O", "/dev/null", "http://127.0.0.1/healthz"]
