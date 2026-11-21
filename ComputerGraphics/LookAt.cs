// 메인카메라가 오브젝트를 향하도록 설정하는 함수 LookAt()

using UnityEngine;

public class LookAt : MonoBehaviour {
    public Transform target;
    void Update() {
        transform.LookAt(target);
    }
}
