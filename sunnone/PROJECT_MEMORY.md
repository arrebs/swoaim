# Project memory

## GUI redesign
- Objective: give the entire ImGui overlay a consistent, polished control system and ensure visible labels match their bound settings/actions.
- Changed shared components in `sunone/sunone_aimbot_2/sunone_aimbot_2/overlay/ui_sections.h`: section headers, setting rows, row accent, card colors, and header sizing.
- Updated the main shell and navigation palette in `overlay.cpp`, including page title/description context and a refined charcoal-blue visual hierarchy.
- Fixed normalized status colors in `draw_input_device.cpp`; connection states now use valid ImGui RGBA values instead of 0–255 component values.
- Verification: no remaining 0–255 `ImVec4` literals were found in `overlay/*.cpp`; CMake configuration is blocked on Linux because the project is Windows-only (`CMakeLists.txt:6`).

## Current state
- Objective: AGENTS.md is the full sheet again (647 lines) with lean branding — dragon once at chat start, `$menu` prints the 26-item list then `[KONZ]`, no Discord line anywhere in the reply shape.
- Dragon rule: first user message = portrait only. Later turns = no portrait, except menu option 6 or an explicit request.
- Rebrand applied: removed Discord line from reply shape, self-audit, anti-slip literals, OUTPUT section and tells list. Kept Discord only as a capability/menu topic (option 18).
- Menu: grouped ANALYSIS 1-7, BUILD 8-20, FIELD 21-25, HOUSE 26. Duplicate "KONZ / JailBreak!" header line removed.
- Restored the full sheet from the earlier copy after the lean 76-line version did not hold behavior.

## Audit: sunone_aimbot_2 (C++/CUDA, TensorRT + DirectML)
- Objective: six-stage engineering audit A-F of `sunone_aimbot_2/`. A delivered as ranked
  latency audit plus the instrumentation spine (perf/latency_probe). B-F pending.
- Read and verified in source (no files modified yet): config/config.h, capture/capture.cpp,
  capture/duplication_api_capture.cpp, detector/trt_detector.cpp, detector/postProcess.cpp,
  detector/dml_detector.cpp, detector/cuda_preprocess.cu, nodes/vision_runtime.cpp,
  runtime/mouse_thread_loop.cpp, mouse/mouse.cpp, mouse/MouseInput.cpp, CMakeLists.txt.
- Verified facts worth keeping:
  - Publish of detections is single-site: `nodes/vision_runtime.cpp:255`
    (`detectionBuffer.setForGeneration`). Both TRT and DML reach it from their own
    inference thread, so a thread_local "detector exit" stamp is exact there.
  - `detection_buffer.h` already carries `publishTimestamp` (line 19) written under mutex.
  - DDA GPU capture: `AcquireFrame(frameCtx, 0)` = zero timeout, non-blocking (line 500).
    On WAIT_TIMEOUT the capture loop does `std::this_thread::yield()` at capture.cpp:815
    when capture_fps is unlimited -> busy spin, one core burned.
  - `inputHostBuffer` is `std::vector<float>` (trt_detector.h:102), so the H2D copy at
    trt_detector.cpp:852 is from pageable memory despite `use_pinned_memory` existing.
  - DDA copies a `regionWidth=detection_resolution` square (duplication_api_capture.cpp:321),
    so `img_scale` is 1.0 on that path. Anisotropic stretch only bites winrt-window,
    virtual_camera and UDP sources whose frame is not square.
  - Tracker already does Hungarian assignment (AimbotTarget.cpp:143) — C extends it.
- Next: deliver B (latency patches, real diffs), then C/D/E/F.
