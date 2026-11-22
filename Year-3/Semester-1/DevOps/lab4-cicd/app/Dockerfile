FROM node:24.22.2-alpine3.24

ARG APP_VERSION=dev
LABEL org.opencontainers.image.title="hello-devops-api" \
      org.opencontainers.image.description="ЛР4 Технології DevOps: сервіс для CI/CD і Kubernetes" \
      org.opencontainers.image.version="${APP_VERSION}" \
      org.opencontainers.image.source="https://github.com/d3Par1/University-Repository"

ENV NODE_ENV=production \
    APP_VERSION=${APP_VERSION} \
    PORT=8080

# Оновлення пакетів Alpine (як у ЛР2) і видалення npm/yarn: залежностей під час роботи
# сервіс не має, а менеджер пакетів лише додає вразливості до образу.
RUN apk upgrade --no-cache \
 && rm -rf /usr/local/lib/node_modules/npm /usr/local/lib/node_modules/corepack \
           /usr/local/bin/npm /usr/local/bin/npx /usr/local/bin/corepack /opt/yarn-*

WORKDIR /app
COPY package.json ./
COPY src/ src/

USER 1000:1000
EXPOSE 8080
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
  CMD ["wget", "-q", "-O", "/dev/null", "http://127.0.0.1:8080/healthz"]
CMD ["node", "src/server.js"]
