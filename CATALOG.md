# Flipper Apps Catalog readiness

Checklist against the official [Contribution Guide](https://github.com/flipperdevices/flipper-application-catalog/blob/main/documentation/Contributing.md).

## Source repository requirements

| Requirement | Status | Notes |
|---|---|---|
| Public GitHub repository | Done | https://github.com/databrainsco/card_scope_zero |
| Open Source license | Done | `LICENSE` (MIT) |
| Builds with uFBT on Release/RC | Done | Official firmware **1.5.1** |
| `application.fam` with unique `appid` | Done | `cardscope_zero` |
| `fap_version` major.minor | Done | `0.1` |
| `fap_category` matches catalog category | Done | `NFC` |
| `fap_description` / author / weburl | Done | Set in `application.fam` |
| 10×10 1-bit PNG icon | Done | `cardscope_zero_10px.png` |
| `README.md` (English) | Done | Usage, build, limits |
| `changelog.md` catalog format | Done | Root + `docs/changelog.md` |
| Screenshots (qFlipper format) | Done | `screenshots/ss0.png` … `ss3.png` (512×256 RGB) |
| No malicious / illegal bypass code | Done | Read-only identify; no auth bypass |
| Does not modify stock firmware | Done | External FAP only |

## Catalog PR package

Draft manifest: [`catalog/manifest.yml`](catalog/manifest.yml)

Target path in catalog repo:

```text
applications/NFC/cardscope_zero/manifest.yml
```

### Submit steps

1. Push the latest source commit to GitHub
2. Put the full commit SHA into `catalog/manifest.yml` → `commit_sha`
3. Fork [flipper-application-catalog](https://github.com/flipperdevices/flipper-application-catalog)
4. Copy `catalog/manifest.yml` to `applications/NFC/cardscope_zero/manifest.yml`
5. Validate:

```bash
python3 tools/bundle.py --nolint applications/NFC/cardscope_zero/manifest.yml bundle.zip
```

6. Open a PR with branch name like `databrainsco/cardscope_zero_0.1`

## Recommended before PR

- Re-capture screenshots with **qFlipper** while the app is running (same 512×256 files, replace in place)
- Confirm `ufbt launch` still works on current Release firmware
- Ensure contact email/GitHub notifications are watched (14-day response rule)
