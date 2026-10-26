# Isolated fixtures verify the evidence reader, never client acceptance.
$ErrorActionPreference = 'Stop'
$reader = Join-Path $PSScriptRoot 'Test-EditModeResult.ps1'
$fixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ('cn00-review-' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $fixtureRoot | Out-Null
$cases = @(
    @{ Name='passed'; Xml='<test-run result="Passed" total="2" passed="2" failed="0" skipped="0"><test-case result="Passed"/><test-case result="Passed"/></test-run>'; Expected=0 },
    @{ Name='zero'; Xml='<test-run result="Passed" total="0" passed="0" failed="0" skipped="0"/>'; Expected=2 },
    @{ Name='failed'; Xml='<test-run result="Failed" total="1" passed="0" failed="1" skipped="0"><test-case result="Failed"/></test-run>'; Expected=2 },
    @{ Name='skipped'; Xml='<test-run result="Passed" total="1" passed="0" failed="0" skipped="1"><test-case result="Skipped"/></test-run>'; Expected=2 },
    @{ Name='inconsistent'; Xml='<test-run result="Passed" total="1" passed="1" failed="0" skipped="0"><test-case result="Failed"/></test-run>'; Expected=2 },
    @{ Name='malformed'; Xml='not xml'; Expected=2 },
    @{ Name='missing'; Xml=$null; Expected=2 }
)
try {
    foreach ($case in $cases) {
        $fixture = Join-Path $fixtureRoot ($case.Name + '.xml')
        if ($null -ne $case.Xml) { Set-Content -LiteralPath $fixture -Value $case.Xml }
        $output = & (Join-Path $PSHOME 'pwsh.exe') -NoProfile -File $reader -ResultPath $fixture 2>&1
        $actual = $LASTEXITCODE
        if ($actual -ne $case.Expected) { throw "$($case.Name): expected exit $($case.Expected), actual $actual. $output" }
        $record = ($output -join "`n") | ConvertFrom-Json
        if ($record.Verified -ne ($case.Expected -eq 0)) { throw "$($case.Name): wrong Verified flag" }
        Write-Output "PASS $($case.Name)"
    }
    Write-Output '7 evidence-reader tests passed; these are not Unity tests.'
} finally {
    # Delete only the uniquely created fixture files, without recursive deletion.
    Get-ChildItem -LiteralPath $fixtureRoot -File | Remove-Item
    Remove-Item -LiteralPath $fixtureRoot
}

