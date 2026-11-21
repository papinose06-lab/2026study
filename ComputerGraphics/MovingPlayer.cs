// 키보드 입력에 따른 오브젝트 translate, rotate

using UnityEngine; 

public class MovingPlayer : MonoBehaviour { 
    public float moveSpeed = 5f; 
    public float jumpSpeed =10f;
    public float turnSpeed = 45f; 

    public Transform LeftLeg;  // 추후 팔다리 동작 구현을 위해 선언
    public Transform RightLeg;
    public Transform LeftArm;
    public Transform RightArm;

    void Update() { 
        if (Input.GetKey(KeyCode.UpArrow)) {
            transform.Translate(Vector3.forward*moveSpeed*Time.deltaTime);
        }
        if (Input.GetKey(KeyCode.DownArrow)) {
            transform.Translate(-Vector3.forward*moveSpeed*Time.deltaTime);
        }
        if (Input.GetKeyDown(KeyCode.LeftArrow)) {
            transform.Rotate(Vector3.up, -turnSpeed);
        }
        if (Input.GetKeyDown(KeyCode.RightArrow)) {
            transform.Rotate(Vector3.up, turnSpeed); 
        }
        if (Input.GetKeyDown(KeyCode.Space)) {
            GetComponent<Rigidbody>().AddForce(Vector3.up*jumpSpeed, ForceMode.Impulse);
        }
    }
} 