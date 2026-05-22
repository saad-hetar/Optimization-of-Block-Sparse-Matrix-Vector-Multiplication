# --- Configuration ---
$ExecutableName = "solver_benchmark.exe"
$OutputFile = "performance_results_full.csv"

# Get the directory where this script is actually located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Executable = Join-Path $ScriptDir $ExecutableName
$OutputFilePath = Join-Path $ScriptDir $OutputFile

# --- Step 1: Compiling Project ---
Write-Host "--- Step 1: Compiling Project ---" -ForegroundColor Cyan
# Run compiler inside the script's directory
Push-Location $ScriptDir
g++ -fopenmp -O3 -mavx2 -mfma -march=native main.cpp block_matrix.cpp vec.cpp -o $ExecutableName
Pop-Location

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Compilation failed." -ForegroundColor Red
    exit
}
Write-Host "Compilation successful!`n" -ForegroundColor Green

# --- Step 2: Initialize CSV file ---
"p(Threads),t(Time),S(Speedup),E(Efficiency),Alpha" | Out-File -FilePath $OutputFilePath -Encoding utf8

# --- Step 3: Running Baseline (1 Thread) ---
Write-Host "--- Step 3: Running Baseline (1 Thread) ---" -ForegroundColor Yellow
$env:OMP_NUM_THREADS = 1

# Execute and merge standard output + error stream (2>&1), forcing array context
# Add this line right before $Output = & $Executable 2>&1
Set-Location -Path "C:\Users\saad_hetar\OneDrive\Desktop\c++"
[array]$Output = & $Executable 2>&1

if ($null -eq $Output -or $Output.Count -eq 0) {
    Write-Host "ERROR: Program ran but returned absolutely no output text!" -ForegroundColor Red
    exit
}

# Find the absolute last line that isn't completely empty or white space
$LastLine = $Output | Where-Object { $_ -and $_.ToString().Trim() -ne "" } | Select-Object -Last 1

if ($null -eq $LastLine) {
    Write-Host "ERROR: Output was generated, but no valid lines could be parsed." -ForegroundColor Red
    exit
}

$T1_String = $LastLine.ToString().Trim()
$T1 = 0.0

if ([double]::TryParse($T1_String, [ref]$T1)) {
    Write-Host "Baseline T1 captured: $T1 seconds" -ForegroundColor Green
} else {
    Write-Host "ERROR: Program output '$T1_String' is not a valid double!" -ForegroundColor Red
    Write-Host "--- Full Dump of Program Output For Debugging ---" -ForegroundColor DarkGray
    $Output | Write-Host -ForegroundColor DarkGray
    exit
}

"1,$T1,1.0000,1.0000,-" | Out-File -FilePath $OutputFilePath -Append -Encoding utf8

# --- Step 4: Multi-threaded Tests ---
Write-Host "`n--- Step 4: Multi-threaded Tests ---" -ForegroundColor Cyan
for ($p = 2; $p -le 10; $p++) {
    Write-Host "Testing with $p threads..." -ForegroundColor Yellow
    $env:OMP_NUM_THREADS = $p

    [array]$Result = & $Executable 2>&1
    
    if ($null -eq $Result -or $Result.Count -eq 0) {
        Write-Host "Warning: No output received for thread count $p." -ForegroundColor Yellow
        continue
    }

    $ResultLine = $Result | Where-Object { $_ -and $_.ToString().Trim() -ne "" } | Select-Object -Last 1
    if ($null -eq $ResultLine) {
        Write-Host "Warning: Could not isolate last line for thread count $p." -ForegroundColor Yellow
        continue
    }

    $Tp_String = $ResultLine.ToString().Trim()
    $Tp = 0.0

    if ([double]::TryParse($Tp_String, [ref]$Tp)) {
        if ($Tp -gt 0) {
            # Amdahl's Law calculations
            $S = $T1 / $Tp
            $E = $S / $p
            $Alpha = ((1/$S) - (1/$p)) / (1 - (1/$p))

            $Sf = "{0:N4}" -f $S
            $Ef = "{0:N4}" -f $E
            $Af = "{0:N4}" -f $Alpha

            "$p,$Tp,$Sf,$Ef,$Af" | Out-File -FilePath $OutputFilePath -Append -Encoding utf8
            Write-Host "Done: $p Threads | Time: $Tp s | Speedup: $Sf" -ForegroundColor Green
        } else {
            Write-Host "Warning: Thread $p reported 0 seconds (division by zero protection triggered)." -ForegroundColor Yellow
        }
    } else {
        Write-Host "Warning: Could not parse output for thread $p. Got: '$Tp_String'" -ForegroundColor Yellow
    }
}

Write-Host "`nBenchmark complete! Results saved to: $OutputFilePath" -ForegroundColor Cyan