# DevOps Pipeline Demo

[![CI](https://github.com/d3Par1/devops-lab1/actions/workflows/main.yml/badge.svg)](https://github.com/d3Par1/devops-lab1/actions/workflows/main.yml)
[![lab2-docker](https://github.com/d3Par1/devops-lab1/actions/workflows/lab2-docker.yml/badge.svg)](https://github.com/d3Par1/devops-lab1/actions/workflows/lab2-docker.yml)

Лабораторні роботи № 1 і № 2 з дисципліни «Технології DevOps».
Виконав: Степаненко Назар Юрійович, ТВ-43, № 14 у списку групи.

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
