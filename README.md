## Convex Hull with GPGPU
![image](./images/convex.png)
### 실행 방법
* 소스코드의 실행을 위해 GLAD, GLM을 include 폴더 안에 추가해야 합니다.
  * [glm 다운로드](https://github.com/g-truc/glm), [glad 다운로드](https://glad.dav1d.de/)
* Convex 폴더의 main.cpp를 실행하면 CPU를 통해 ConvexHull을 만들게 됩니다.
* ConvexGPU 폴더의 main.cpp를 실행하면 GPU를 통해 ConvexHull을 만들게 됩니다. 