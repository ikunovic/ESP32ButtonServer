# Simple GitHub Setup Script for ESP32ButtonServer
# ------------------------------------------

# Ask for GitHub username
$githubUsername = Read-Host "Enter your GitHub username"

# Repository name
$repoName = "ESP32ButtonServer"

# Confirm details
Write-Host "`nYou entered:" -ForegroundColor Cyan
Write-Host "GitHub Username: $githubUsername" -ForegroundColor Cyan
Write-Host "Repository Name: $repoName" -ForegroundColor Cyan
$confirm = Read-Host "`nIs this correct? (y/n)"

if ($confirm -ne "y") {
    Write-Host "Setup cancelled. Please run the script again with correct information." -ForegroundColor Red
    exit
}

# Step 1: Initialize git in the current directory
Write-Host "`nInitializing Git repository..." -ForegroundColor Green
git init

# Step 2: Create .gitignore file
Write-Host "Creating .gitignore file..." -ForegroundColor Green
@"
.pio
.vscode/.browse.c_cpp.db*
.vscode/c_cpp_properties.json
.vscode/launch.json
.vscode/ipch
src/wifi_credentials.h
"@ | Out-File -FilePath ".gitignore" -Encoding utf8

# Step 3: Add and commit all files
Write-Host "Adding and committing files..." -ForegroundColor Green
git add .
git commit -m "Initial commit"

# Step 4: Create GitHub repository
Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "1. Go to https://github.com/new" -ForegroundColor Yellow
Write-Host "2. Create a new repository named $repoName" -ForegroundColor Yellow
Write-Host "3. Make sure NOT to initialize it with README, license, or .gitignore" -ForegroundColor Yellow
Write-Host "4. Click 'Create repository'" -ForegroundColor Yellow
Write-Host "5. After creating the repository, run the following commands:" -ForegroundColor Yellow

# Step 5: Show commands to push to GitHub
Write-Host "`nCommands to run after creating the repository:" -ForegroundColor Cyan
Write-Host "git remote add origin https://github.com/$githubUsername/$repoName.git" -ForegroundColor White
Write-Host "git branch -M main" -ForegroundColor White 
Write-Host "git push -u origin main" -ForegroundColor White

# Optional: Ask if user wants to set up GitHub Pages in a separate branch
Write-Host "`nWould you like to set up GitHub Pages in a separate branch?" -ForegroundColor Yellow
$setupPages = Read-Host "This will create a gh-pages branch with just the web-portal files (y/n)"

if ($setupPages -eq "y") {
    Write-Host "`nAfter pushing your main branch, run these commands:" -ForegroundColor Cyan
    Write-Host "git checkout -b gh-pages" -ForegroundColor White
    Write-Host "git rm -rf ." -ForegroundColor White
    Write-Host "git checkout main -- web-portal" -ForegroundColor White
    Write-Host "mkdir temp" -ForegroundColor White
    Write-Host "mv web-portal/* temp/" -ForegroundColor White
    Write-Host "rm -rf web-portal" -ForegroundColor White
    Write-Host "mv temp/* ." -ForegroundColor White
    Write-Host "rm -rf temp" -ForegroundColor White
    Write-Host "git add ." -ForegroundColor White
    Write-Host "git commit -m 'Set up GitHub Pages'" -ForegroundColor White
    Write-Host "git push -u origin gh-pages" -ForegroundColor White
    Write-Host "git checkout main" -ForegroundColor White
    
    Write-Host "`nThen go to your repository settings > Pages and select gh-pages branch as the source" -ForegroundColor Yellow
}

Write-Host "`nScript complete. Follow the instructions above to finish the GitHub setup." -ForegroundColor Green 