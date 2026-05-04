# build-docx.ps1 — конвертує zvit-temp.html у Звіт_ПР5.docx через Word COM
# Word відкриває HTML як документ, конвертує внутрішньо у docx-формат

$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Definition
$htmlPath = Join-Path $here 'zvit-temp.html'
$docxPath = Join-Path $here 'Звіт_ПР5.docx'

if (-not (Test-Path $htmlPath)) {
    Write-Error "HTML не знайдено: $htmlPath"
    exit 1
}

Write-Output "Запуск Word COM..."
$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0  # wdAlertsNone

try {
    Write-Output "Відкриваю HTML..."
    # ConfirmConversions=$false щоб Word не питав про конвертацію
    $doc = $word.Documents.Open($htmlPath, $false, $true)   # readOnly=true для безпеки

    Write-Output "Зберігаю як .docx..."
    # 16 = wdFormatDocumentDefault (.docx)
    $doc.SaveAs2($docxPath, 16)
    $doc.Close($false)
    Write-Output "✓ Створено: $docxPath"

    # Перевіряємо розмір
    $size = (Get-Item $docxPath).Length
    Write-Output ("Розмір файлу: {0:N0} байт ({1:N1} KB)" -f $size, ($size / 1024))
}
finally {
    $word.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
}
