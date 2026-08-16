Libraries can be added to this project in two ways:
- [**Vendor branch**](#vendor-branch): Best for simple libraries (like single-header files) or those without a Git repository.
- [**Subtree**](#subtree): Recommended when integrating an existing Git repository.

# Vendor branch
Add:
```
git checkout --orphan vendor/curl
git rm -rf .
git status
mkdir -p third_party/curl
tar --directory=third_party/curl --extract --file=/path/to/curl-7.80.0.tar.gz --strip-components=1
git add third_party/curl
git commit --message="curl 7.80.0"
git checkout main
git merge vendor/curl
```

Update:
```
git checkout vendor/curl
git rm -rf third_party/curl
mkdir -p third_party/curl
tar --directory=third_party/curl --extract --file=/path/to/curl-7.81.0.tar.gz --strip-components=1
git add third_party/curl
git commit --message="curl 7.81.0"
git checkout main
git merge vendor/curl
```

# Subtree
Add:
```
git subtree add --squash --prefix=libraries/raylib https://github.com/raysan5/raylib.git 6.0
```

Update:
```
git subtree pull --squash --prefix=libraries/raylib https://github.com/raysan5/raylib.git 6.1
```
> This info was taken from: https://david.rothlis.net/vendor-branch/