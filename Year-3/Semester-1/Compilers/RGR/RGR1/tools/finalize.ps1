# Opens the generated specification in Microsoft Word, updates the table of contents
# and all fields, saves the .docx and exports a PDF next to it.
# Usage:  pwsh tools/finalize.ps1
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$docx = Get-ChildItem -LiteralPath $root -Filter '*.docx' | Where-Object { $_.Name -like 'Специфікація*' } | Select-Object -First 1
if (-not $docx) { throw 'Specification .docx not found; run tools/build_spec.py first' }
$pdf = [System.IO.Path]::ChangeExtension($docx.FullName, '.pdf')

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
try {
    $doc = $word.Documents.Open($docx.FullName, $false, $false)
    foreach ($toc in $doc.TablesOfContents) { $toc.Update() }
    $doc.Fields.Update() | Out-Null
    foreach ($toc in $doc.TablesOfContents) { $toc.Update() }
    $doc.Save()
    $doc.ExportAsFixedFormat($pdf, 17)   # wdExportFormatPDF
    Write-Output ("pages: " + $doc.ComputeStatistics(2))
    $doc.Close($false)
    Write-Output "updated $($docx.Name)"
    Write-Output "exported $([System.IO.Path]::GetFileName($pdf))"
}
finally {
    $word.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
}
