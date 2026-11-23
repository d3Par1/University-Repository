# DevOps Pipeline Demo

[![CI](https://github.com/d3Par1/devops-lab1/actions/workflows/main.yml/badge.svg)](https://github.com/d3Par1/devops-lab1/actions/workflows/main.yml)
[![lab2-docker](https://github.com/d3Par1/devops-lab1/actions/workflows/lab2-docker.yml/badge.svg)](https://github.com/d3Par1/devops-lab1/actions/workflows/lab2-docker.yml)

Лабораторні роботи № 1–4 з дисципліни «Технології DevOps».
Виконав: Степаненко Назар Юрійович, ТВ-43, № 14 у списку групи.

Бейджі вище – запуски ЛР1–ЛР2 у репозиторії `devops-lab1`. Конвеєри ЛР3–ЛР4 лежать у корені цього
репозиторію: `.github/workflows/devops-lab3-terraform.yml`, `devops-lab4-minimal.yml`, `devops-lab4-cicd.yml`.

## ЛР4: CI/CD і Kubernetes (`lab4-cicd/`)

Сервіс `hello-devops-api` (Node.js, модульні тести з порогами покриття), маніфести Kubernetes з
перевірками готовності й життєздатності та конвеєр: лінтери й тести → збірка + Trivy →
приймальні тести в кластері kind → публікація в GHCR (лише з `main`).

```bash
cd lab4-cicd/app && npm ci && npm run lint && npm test
docker build --build-arg APP_VERSION=1.0.0 -t hello-devops-api:1.0.0 app   # з каталогу lab4-cicd
kind create cluster --config kind-cluster.yaml
kind load docker-image hello-devops-api:1.0.0 --name lab4
kubectl apply -k k8s && kubectl -n lab4 rollout status deployment/web
curl localhost:8090/api/info
kind delete cluster --name lab4
```

## ЛР3: інфраструктура як код (`lab3-terraform/`)

`01-local` – конфігурація із завдання 3 (`local_file`), `02-docker` – образ ЛР2 у двох середовищах
(staging :8081, production :8082) через провайдер Docker.

```bash
cd lab3-terraform/01-local && terraform init && terraform apply && terraform destroy
cd ../02-docker && terraform init && terraform apply   # потім terraform destroy
```

## ЛР2: контейнеризація (`lab2-docker/`)

Двоетапний образ nginx без root із HEALTHCHECK, `compose.yaml` зі smoke-тестом і конвеєр
`.github/workflows/lab2-docker.yml`, що публікує перевірений образ у GHCR:

```bash
docker run -d -p 8080:80 ghcr.io/d3par1/hello-devops:2.0.2      # готовий образ з реєстру
docker compose -f lab2-docker/compose.yaml up -d --build --wait  # або зібрати локально
docker compose -f lab2-docker/compose.yaml --profile test run --rm smoke
```

## ЛР1

| Завдання | Що в репозиторії |
|---|---|
| 1. Git: конфлікт злиття | `info.txt`, гілка `feature/user-data`, коміт злиття `5ef0f65` |
| 2. Docker | `Dockerfile`, `index.html`, `.dockerignore` |
| 3. Terraform | `terraform/main.tf`, `terraform/.terraform.lock.hcl` |
| 4. GitHub Actions | `.github/workflows/main.yml` |

## Запуск

```bash
# Завдання 2: образ і контейнер, сторінка на http://localhost:8080
docker build -t my-app:latest .
docker run -d --name devops-web -p 8080:80 my-app:latest

# Завдання 3: файл devops_iac_output.txt через Terraform
cd terraform
terraform init
terraform plan
terraform apply
terraform destroy
```

Конвеєр CI запускається на кожен push у `main`: спершу `Test/Lint` (hadolint для Dockerfile),
потім `Build/Package` (`docker build -t my-app:latest .` і перевірка сторінки в контейнері).
