"""Extract HECA Free Text coordinates from a sector .ese file."""
import re, json, sys
from pathlib import Path

if len(sys.argv) != 3:
    print("Usage: python extract_heca_freetext.py sector.ese HECA_Gates.json")
    raise SystemExit(1)

text = Path(sys.argv[1]).read_text(encoding="utf-8", errors="ignore")
pat = re.compile(r'^(N\d{3}\.\d{2}\.\d+(?:\.\d+)?):(E\d{3}\.\d{2}\.\d+(?:\.\d+)?):HECA Freetext:(.+)$', re.M)

stands = []
for lat, lon, label in pat.findall(text):
    stands.append({"label": label.strip(), "lat_dms": lat, "lon_dms": lon, "radius_m": 32})

Path(sys.argv[2]).write_text(json.dumps({"airport":"HECA","stands":stands}, indent=2), encoding="utf-8")
print(f"Wrote {len(stands)} HECA labels")
