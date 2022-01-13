# Graphics-OpenGL
A personal project to explore and learn the general graphics rendering pipelines. Written in C++ for Windows using OpenGL APIs and GLSL shaders. 
Introduces deferred shading, lit instancing, lights, shadow-mapping with multiple lights, material/texture abstractions, bloom and other post-processing effects. 
Current WIP: Automating the pipeline through in-order draw buckets. 
Future: PBR, cascading shadow maps, physics project integration.
## Scene 0
### Model with diffuse, specular and normal maps.
![screenshot_1](https://user-images.githubusercontent.com/25257875/52543205-a6066e00-2d9f-11e9-8c16-deda00f5aa5a.png)

### Spotlight, Directional light and Point lights
![screenshot_2](https://user-images.githubusercontent.com/25257875/52543206-ab63b880-2d9f-11e9-8cce-c9b798defed5.png)

### VFX (unanimated dot shield) interfering with spotlight (flashlight) turns from blue to red.
![screenshot_3](https://user-images.githubusercontent.com/25257875/52543208-b1599980-2d9f-11e9-9aac-60ff5bfc5846.png)

### Blending
![screenshot_4](https://user-images.githubusercontent.com/25257875/52543209-b4548a00-2d9f-11e9-8274-9ffb33c9edf4.png)

### Instancing (meteors) and Cubemap (outerspace)
![screenshot_5](https://user-images.githubusercontent.com/25257875/52543213-bd455b80-2d9f-11e9-94f8-e3e2d8e68391.png)

### Instancing (close up)
![screenshot_6](https://user-images.githubusercontent.com/25257875/52543216-c2a2a600-2d9f-11e9-965c-34e0ada29adc.png)

### Post-processing: Blur, Edge-detection, Grayscale with green color correction.
![blur](https://user-images.githubusercontent.com/25257875/52543461-c0d9e200-2da1-11e9-925e-baca92e570d4.png)
![edgedetection](https://user-images.githubusercontent.com/25257875/52543465-cc2d0d80-2da1-11e9-8e96-d1a85828c14f.png)
![grayscalefull](https://user-images.githubusercontent.com/25257875/52543471-d5b67580-2da1-11e9-9951-8b2eeb80c4dc.png)

## Scene 1
### Directional shadow-mapping with smoothing.
![dir_shadow_map](https://user-images.githubusercontent.com/25257875/52543504-1b733e00-2da2-11e9-98be-bf91ace52041.png)
![dir_shadow_map2](https://user-images.githubusercontent.com/25257875/52543506-1e6e2e80-2da2-11e9-921c-96b2dc9a1d3b.png)

## Scene 2  
### Normal omnidirectional shadow-mapping (with moving Point Light)
![screenshot_7](https://user-images.githubusercontent.com/25257875/52543537-51b0bd80-2da2-11e9-8dfd-3e2377dc5176.png)

### Smooth omnidirectional shadow-mapping (with moving Point Light)
![smooth_omni](https://user-images.githubusercontent.com/25257875/52543550-760c9a00-2da2-11e9-8ce4-856f73a85ae2.png)
![screenshot_8](https://user-images.githubusercontent.com/25257875/52543553-7d33a800-2da2-11e9-943a-6daa21604bd8.png)
![screenshot_9](https://user-images.githubusercontent.com/25257875/52543557-858be300-2da2-11e9-9df0-0c624ebdb5f0.png)

## Scene 3
### Normal mapping
![normal_mapping](https://user-images.githubusercontent.com/25257875/52543564-9fc5c100-2da2-11e9-8073-b8d41d079435.png)
![normal_mapping2](https://user-images.githubusercontent.com/25257875/52543565-a2281b00-2da2-11e9-92f0-42b7729a9f35.png)

### Paralax mapping with different depths.
![paralax](https://user-images.githubusercontent.com/25257875/52543574-b66c1800-2da2-11e9-88a7-94724b5d186d.png)
![screenshot_10](https://user-images.githubusercontent.com/25257875/52543579-bf5ce980-2da2-11e9-89b1-c45ab02373ea.png)
![screenshot_12](https://user-images.githubusercontent.com/25257875/52543581-c71c8e00-2da2-11e9-8f87-91cdb7979ed3.png)

## Scene 4
### HDR Off / On 
![hdr_off](https://user-images.githubusercontent.com/25257875/52543604-f3d0a580-2da2-11e9-9c57-89845333f4d7.png)
![hdr_exposure_3](https://user-images.githubusercontent.com/25257875/52543608-ffbc6780-2da2-11e9-8005-2c24d0c47b63.png)

## Scene 5
### Bloom
![bloom](https://user-images.githubusercontent.com/25257875/52543636-1793eb80-2da3-11e9-8d71-cab69c27f217.png)
![bloom2](https://user-images.githubusercontent.com/25257875/52543637-18c51880-2da3-11e9-9e75-1b4405fb4816.png)

