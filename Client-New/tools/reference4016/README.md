# Reference 4016 metadata-only analysis

Source-controlled tooling to inspect the user's locally installed 4016 UnityFS bundles and IL2CPP metadata **without executing/modifying the reference client** or writing extracted mesh/texture/script binaries. No server access.

## Environment

- Python 3.13 used in the measured run.
- UnityPy 1.25.4 via `python -m pip install UnityPy==1.25.4 --target "<root>\.tmp\reference4016-python-deps" --no-input`.
- Dependency folder is under ignored `.tmp` and **must not be committed**.
- No native IL2CPP decompiler has been installed or used; metadata_symbol_hints.py is bounded identifier-string scanning only.

## Scripts

- `probe_4016.py`: object-type counts and selected schema examples.
- `probe_object_files.py`: distinguishes multiple serialized files within a UnityFS bundle (important for avoiding path ID collisions).
- `scene_transform_inventory.py`: reads the scene GameObject/Transform relationships and composes X/Y/Z world Transform positions. It does not export meshes.
- `mesh_bounds_inventory.py`: scene Mesh AABBs, mesh references with serialized-file scoping; does not export mesh vertices.
- `make_reference_candidates.py`: four real scene terrain tile transform origins with no invented legacy coordinates, status UNPAIRED_REFERENCE_SCENE_ONLY.
- `metadata_symbol_hints.py`: filtered names from global-metadata.dat, **not** implementations.
- `verify_4016_inventories.py`: 8 static analytical regression checks; no Unity Editor invocation.

Full input and output paths, SHA256, limitations and reproduced commands are in `reports/CN02_4016_STATIC_ANALYSIS.md`. Outputs are analytical metadata under `reports/evidence/cn02-4016-static/`, not reusable runtime assets. Do not import proprietary geometry into Client-New or push to a public repository without establishing the appropriate usage rights.

**Do not** merge reference scene coordinates with the synthetic Client-New scene or mark CN02-04 PASS until independent 3+1 matched legacy observations and holdout validation are available.
