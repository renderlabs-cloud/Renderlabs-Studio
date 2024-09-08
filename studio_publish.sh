#!/bin/bash

token=$(cat /c/msys64/home/Mike/secret.txt)
read -p "Enter commit message: " COMMIT_MSG

if [ ! -d ".git" ]; then
    echo "Git repository not found. Please initialize a Git repository."
    exit 1
fi

git add .
git rm .gitignore

git commit -m "$COMMIT_MSG"

git rm -rf --cached .
git add .

repo_url="https://github.com/renderlabs-cloud/Renderlabs-Studio.git"
auth_url=$(echo $repo_url | sed "s#https://#https://$token@#")

git remote set-url origin $auth_url

# Push changes
git push -u origin master --force
sleep 10000000000