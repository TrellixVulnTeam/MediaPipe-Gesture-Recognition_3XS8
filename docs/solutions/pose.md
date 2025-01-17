---
layout: default
title: Pose
parent: Solutions
nav_order: 5
---

# MediaPipe Pose
{: .no_toc }

1. TOC
{:toc}
---

## Overview

Human pose estimation from video plays a critical role in various applications
such as quantifying physical exercises, sign language recognition, and full-body
gesture control. For example, it can form the basis for yoga, dance, and fitness
applications. It can also enable the overlay of digital content and information
on top of the physical world in augmented reality.

MediaPipe Pose is a ML solution for high-fidelity upper-body pose tracking,
inferring 25 2D upper-body landmarks from RGB video frames utilizing our
[BlazePose](https://mediapipe.page.link/blazepose-blog) research. Current
state-of-the-art approaches rely primarily on powerful desktop environments for
inference, whereas our method achieves real-time performance on most modern
[mobile phones](#mobile), [desktops/laptops](#desktop), in [python](#python) and
even on the [web](#web). A variant of MediaPipe Pose that performs full-body
pose tracking on mobile phones will be included in an upcoming release of
[ML Kit](https://developers.google.com/ml-kit/early-access/pose-detection).

![pose_tracking_upper_body_example.gif](../images/mobile/pose_tracking_upper_body_example.gif) |
:--------------------------------------------------------------------------------------------: |
*Fig 1. Example of MediaPipe Pose for upper-body pose tracking.*                               |

## ML Pipeline

The solution utilizes a two-step detector-tracker ML pipeline, proven to be
effective in our [MediaPipe Hands](./hands.md) and
[MediaPipe Face Mesh](./face_mesh.md) solutions. Using a detector, the pipeline
first locates the pose region-of-interest (ROI) within the frame. The tracker
subsequently predicts the pose landmarks within the ROI using the ROI-cropped
frame as input. Note that for video use cases the detector is invoked only as
needed, i.e., for the very first frame and when the tracker could no longer
identify body pose presence in the previous frame. For other frames the pipeline
simply derives the ROI from the previous frame’s pose landmarks.

The pipeline is implemented as a MediaPipe
[graph](https://github.com/google/mediapipe/tree/master/mediapipe/graphs/pose_tracking/upper_body_pose_tracking_gpu.pbtxt)
that uses a
[pose landmark subgraph](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_landmark/pose_landmark_upper_body_gpu.pbtxt)
from the
[pose landmark module](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_landmark)
and renders using a dedicated
[upper-body pose renderer subgraph](https://github.com/google/mediapipe/tree/master/mediapipe/graphs/pose_tracking/subgraphs/upper_body_pose_renderer_gpu.pbtxt).
The
[pose landmark subgraph](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_landmark/pose_landmark_upper_body_gpu.pbtxt)
internally uses a
[pose detection subgraph](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_detection/pose_detection_gpu.pbtxt)
from the
[pose detection module](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_detection).

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

## Models

### Pose Detection Model (BlazePose Detector)

The detector is inspired by our own lightweight
[BlazeFace](https://arxiv.org/abs/1907.05047) model, used in
[MediaPipe Face Detection](./face_detection.md), as a proxy for a person
detector. It explicitly predicts two additional virtual keypoints that firmly
describe the human body center, rotation and scale as a circle. Inspired by
[Leonardo’s Vitruvian man](https://en.wikipedia.org/wiki/Vitruvian_Man), we
predict the midpoint of a person's hips, the radius of a circle circumscribing
the whole person, and the incline angle of the line connecting the shoulder and
hip midpoints.

![pose_tracking_detector_vitruvian_man.png](../images/mobile/pose_tracking_detector_vitruvian_man.png) |
:----------------------------------------------------------------------------------------------------: |
*Fig 2. Vitruvian man aligned via two virtual keypoints predicted by BlazePose detector in addition to the face bounding box.* |

### Pose Landmark Model (BlazePose Tracker)

The landmark model currently included in MediaPipe Pose predicts the location of
25 upper-body landmarks (see figure below), with three degrees of freedom each
(x, y location and visibility), plus two virtual alignment keypoints. It shares
the same architecture as the full-body version that predicts 33 landmarks,
described in more detail in the
[BlazePose Google AI Blog](https://mediapipe.page.link/blazepose-blog) and in
this [paper](https://arxiv.org/abs/2006.10204).

![pose_tracking_upper_body_landmarks.png](../images/mobile/pose_tracking_upper_body_landmarks.png) |
:------------------------------------------------------------------------------------------------: |
*Fig 3. 25 upper-body pose landmarks.*                                                             |

## Example Apps

Please first see general instructions for
[Android](../getting_started/building_examples.md#android),
[iOS](../getting_started/building_examples.md#ios),
[desktop](../getting_started/building_examples.md#desktop) and
[Python](../getting_started/building_examples.md#python) on how to build
MediaPipe examples.

Note: To visualize a graph, copy the graph and paste it into
[MediaPipe Visualizer](https://viz.mediapipe.dev/). For more information on how
to visualize its associated subgraphs, please see
[visualizer documentation](../tools/visualizer.md).

### Mobile

*   Graph:
    [`mediapipe/graphs/pose_tracking/upper_body_pose_tracking_gpu.pbtxt`](https://github.com/google/mediapipe/tree/master/mediapipe/graphs/pose_tracking/upper_body_pose_tracking_gpu.pbtxt)
*   Android target:
    [(or download prebuilt ARM64 APK)](https://drive.google.com/file/d/1uKc6T7KSuA0Mlq2URi5YookHu0U3yoh_/view?usp=sharing)
    [`mediapipe/examples/android/src/java/com/google/mediapipe/apps/upperbodyposetrackinggpu:upperbodyposetrackinggpu`](https://github.com/google/mediapipe/tree/master/mediapipe/examples/android/src/java/com/google/mediapipe/apps/upperbodyposetrackinggpu/BUILD)
*   iOS target:
    [`mediapipe/examples/ios/upperbodyposetrackinggpu:UpperBodyPoseTrackingGpuApp`](http:/mediapipe/examples/ios/upperbodyposetrackinggpu/BUILD)

### Desktop

Please first see general instructions for
[desktop](../getting_started/building_examples.md#desktop) on how to build
MediaPipe examples.

*   Running on CPU
    *   Graph:
        [`mediapipe/graphs/pose_tracking/upper_body_pose_tracking_cpu.pbtxt`](https://github.com/google/mediapipe/tree/master/mediapipe/graphs/pose_tracking/upper_body_pose_tracking_cpu.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/upper_body_pose_tracking:upper_body_pose_tracking_cpu`](https://github.com/google/mediapipe/tree/master/mediapipe/examples/desktop/upper_body_pose_tracking/BUILD)
*   Running on GPU
    *   Graph:
        [`mediapipe/graphs/pose_tracking/upper_body_pose_tracking_gpu.pbtxt`](https://github.com/google/mediapipe/tree/master/mediapipe/graphs/pose_tracking/upper_body_pose_tracking_gpu.pbtxt)
    *   Target:
        [`mediapipe/examples/desktop/upper_body_pose_tracking:upper_body_pose_tracking_gpu`](https://github.com/google/mediapipe/tree/master/mediapipe/examples/desktop/upper_body_pose_tracking/BUILD)

### Python

Please first see general instructions for
[Python](../getting_started/building_examples.md#python) examples.

```bash
(mp_env)$ python3
>>> import mediapipe as mp
>>> pose_tracker = mp.examples.UpperBodyPoseTracker()

# For image input
>>> pose_landmarks, _ = pose_tracker.run(input_file='/path/to/input/file', output_file='/path/to/output/file')
>>> pose_landmarks, annotated_image = pose_tracker.run(input_file='/path/to/file')

# For live camera input
# (Press Esc within the output image window to stop the run or let it self terminate after 30 seconds.)
>>> pose_tracker.run_live()

# Close the tracker.
>>> pose_tracker.close()
```

### Web

Please refer to [these instructions](../index.md#mediapipe-on-the-web).

## Resources

*   Google AI Blog:
    [BlazePose - On-device Real-time Body Pose Tracking](https://mediapipe.page.link/blazepose-blog)
*   Paper:
    [BlazePose: On-device Real-time Body Pose Tracking](https://arxiv.org/abs/2006.10204)
    ([presentation](https://youtu.be/YPpUOTRn5tA))
*   Pose detection model:
    [TFLite model](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_detection/pose_detection.tflite)
*   Upper-body pose landmark model:
    [TFLite model](https://github.com/google/mediapipe/tree/master/mediapipe/modules/pose_landmark/pose_landmark_upper_body.tflite)
*   [Model card](https://mediapipe.page.link/blazepose-mc)
