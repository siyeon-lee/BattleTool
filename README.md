# BattleTool
## 모습
![image](https://github.com/siyeon-lee/BattleTool/assets/44990205/453a7a84-ad94-4719-bc4b-5164123ed4f5)


## 기능
테스트 레벨을 열 때, 필요한 초기 셋팅을 쉽게 하기 위한 툴.  

## 구현
테이블데이터 값의 변경에 따라, 전체적인 골자가 쉽게 바뀔 수 있으므로 배치 및 수정이 빠른 EditorUtilityWidget으로 구현.
단, 디테일창이나 CMD입력창 등 실시간 UObject정보를 띄운다거나 하여 UMG로만 처리가 어려운 부분은 Slate록 구현하고, UserWidget::RebuildWidget()을 오버라이드 하여 UMG로 덮어씌움.

## 지원기능
1. 레벨을 시작할 데이터 셋팅 (캐릭터, 스폰위치, 스킬 등)
2. 데이터 Valid 체크(스킬인덱스, 레벨 이름 등)
  * 올바르지 않은 데이터의 경우는 빨간색으로 표기한다
3. 데이터 버전별 정리
  * 저장 버튼을 누를 시, ini로 설정데이터, commandList, 레벨정보 등을 모두 저장한다
  * Export/Import 버튼을 누를 시, 레벨 시작할 때 넘길 캐릭터 및 셋팅값들만 json으로 저장하여 버전별로 데이터 관리가 용이하며, 서로 정보를 주고받기 쉽다
4. copy/paste
  * 기획 요청으로 만든 기능/ SkillList값만 복사한다.
5. CommandList
콘솔명령어를 입력하는 창은 Slate를 통해 제작하였고, 배치를 쉽게 하기 위해 UMG로 덮어씌움
6. 스킬 디테일 창
스킬별로 디테일뷰를 볼 수 있도록 만든 창. SkillList의 인덱스를 클릭하면 관련 데이터로 포커스가 잡힌다.
