terraform {
  required_version = ">= 1.6"

  required_providers {
    local = {
      source  = "hashicorp/local"
      version = "~> 2.9"
    }
  }
}

# Ресурс із завдання: текстовий файл, яким керує Terraform.
resource "local_file" "devops_file" {
  content  = "Цей файл створено за допомогою Terraform"
  filename = "${path.module}/devops_iac_output.txt"
}

output "file_path" {
  description = "Шлях до створеного файлу"
  value       = local_file.devops_file.filename
}
