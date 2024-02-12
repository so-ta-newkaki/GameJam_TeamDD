/**************************************
**�@��5�́@�~�j�Q�[�������(2)
**�@�J�[���[�X&������Q�[��
**�@2022�N8��
***************************************/
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define RANKING_DATA 5

/**************************************
*�@�񋓌^�̐錾
***************************************/
enum mode {
	TITLE,
	INIT,
	MAIN,
	RANKING,
	HELP,
	INPUTNAME,
	GAMEOVER,
	END,
	CLOSE
};
/**************************************
*�@�ϐ��̐錾
***************************************/
// ��ʗ̈�̑傫��
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// ���@�̏����l�̒萔
const int PLAYER_POS_X = SCREEN_WIDTH / 2;
const int PLAYER_POS_Y = SCREEN_HEIGHT - 100;
const int PLAYER_WIDTH = 20;//63
const int PLAYER_HEIGHT = 60;//120
const int PLAYER_SPEED = 5;
const int PLAYER_HP = 1000;
const int PLAYER_FUEL = 20000;
const int PLAYER_BARRIER = 3;
const int PLAYER_BARRIERUP = 10;

// �G�@�̍ő吔
const int ENEMY_MAX = 8;

// �A�C�e���̍ő�l
const int ITEM_MAX = 3;

/**************************************
*�@�ϐ��̐錾(�O���[�o���ϐ�)
***************************************/
int gOldKey;           // �O��̓��̓L�[
int gNowKey;           // ����̓��̓L�[
int gKeyFlg;           // ���̓L�[���

int gGameMode = TITLE;    // �Q�[�����[�h

int gTitleImg;         // �^�C�g���摜
int gMenuImg;          // ���j���[�摜
int gConeImg;          // ���j���[�J�[�\���摜

int gScore = 0;        // �X�R�A
int gRankingImg;       // �����L���O��ʔw�i

int gItemImg[2];       // �A�C�e���摜

int gWaitTime = 0;      // �҂�����
int gEndImg;            // �G���h�摜

int gMileage;          // ���s����
int gEnemyCount[4];    // �G�J�E���g
int gEnemyImg[4];      // �G�L�����摜

int gStageImg;         // �X�e�[�W�摜(���H)

int gCarImg;           // �Ԃ̉摜
int gBarrierImg;       // �o���A�̉摜

/**************************************
*�@�\����
***************************************/
// �����L���O�f�[�^�\����
struct RankingData {
	int     no;
	char    name[11];
	long    score;

};

// �����L���O�f�[�^�̕ϐ��錾
struct RankingData    gRanking[RANKING_DATA];

// ���@�̍\����
struct PLAYER {
	int flg;          // �g�p�t���O
	int x, y;         // ���W
	int w, h;         // ���E����
	double angle;     // �@�̂̌���
	int count;        // �^�C�~���O�p
	int speed;        // �ړ����x
	int hp;           // �̗�
	int fuel;         // �R��
	int bari;         // �o���A��
	int baricnt;      // �o���A�p������
	int bariup;       // �o���A��(���s�����A�b�v)
};

// ���@�̕ϐ��錾
struct PLAYER gPlayer;

// �G�@�̍\����
struct ENEMY {
	int flg;        // �g�p�t���O
	int type;       // �^�C�v
	int img;        // �摜
	int x, y, w, h; // ���W�A���A����
	int speed;      // �ړ����x
	int point;      // �X�R�A���Z�|�C���g
	int dako;       // �֍s�ʒu
	int d_flg;
};

// �G�@�̕ϐ��錾
struct ENEMY gEnemy[ENEMY_MAX];
struct ENEMY gEnemy00 = { TRUE,0,0,0,-50,63,120,0,1 };
struct ENEMY gEnemyCn = { TRUE,4,0,0,-50,18,18,0,1 };

// �A�C�e���̕ϐ��錾
struct ENEMY gItem[ITEM_MAX];
struct ENEMY gItem00 = { TRUE,0,0,0,-50,50,50,0,1 };

/**************************************
*�@�֐��̃v���g�^�C�v�錾
***************************************/
int LoadImages(void);     // �摜�Ǎ�����
void GameInit(void);      // �Q�[������������

void DrawTitle(void);     // �^�C�g���`�揈��
void DrawMain(void);      // �Q�[�����C���`�揈��
void DrawHelp(void);      // �Q�[���w���v�`�揈��
void DrawRanking(void);   // �����L���O�`�揈��
void DrawOver(void);      // �Q�[���I�[�o�[�`�揈��
void DrawEnd(void);       // �Q�[���G���h�`�揈��

void InputName(void);     // �n�C�X�R�A�̖��O���͏���

void SortRanking(void);   // �����L���O�̕��בւ�
int SaveRanking(void);    // �����L���O�f�[�^�̕ۑ�
int ReadRanking(void);    // �����L���O�f�[�^�̓Ǎ�

void BackScrool(void);    // �w�i�摜�̃X�N���[������

void PlayerControl(void); // �v���C���[����ƃQ�[����ԕ\��

void EnemyControl(void);  // �G�@����
int CreateEnemy(void);    // �G�@��������

int HitBoxPlayer(PLAYER* p, ENEMY* e); // �����蔻��

void ItemControl(void);   // �A�C�e������
int CreateItem(void);     // �A�C�e����������

/**************************************
*�@�v���O�����̊J�n
***************************************/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// �^�C�g����ݒ�
	SetMainWindowText("Drive&Avoid");

	// �E�B���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// DX���C�u�����̏���������
	if (DxLib_Init() == -1)return -1;

	// �`����ʂ𗠂ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �摜�Ǎ��֐��̌Ăяo��
	if (LoadImages() == -1) return -1;

	// �����L���O�f�[�^�̓Ǎ�
	if (ReadRanking() == -1) return -1;

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && gGameMode != CLOSE && !(gKeyFlg & PAD_INPUT_START)) {

		// ���̓L�[�擾
		gOldKey = gNowKey;
		gNowKey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		gKeyFlg = gNowKey & ~gOldKey;

		// ��ʂ̏�����
		ClearDrawScreen();

		//�Q�[�����[�h�Ɖ�ʑJ�ڂ̐���
		switch (gGameMode) {
		case TITLE:
			DrawTitle();
			break;
		case INIT:
			GameInit();
			break;
		case MAIN:
			DrawMain();
			break;
		case RANKING:
			DrawRanking();
			break;
		case HELP:
			DrawHelp();
			break;
		case INPUTNAME:
			InputName();
			break;
		case GAMEOVER:
			DrawOver();
			break;
		case END:
			DrawEnd();
			break;
		}

		// ����ʂ̓��e��\��ʂɔ��f����
		ScreenFlip();
	}

		// DX���C�u�����g�p�̏I������
		DxLib_End();
	
	// �v���O�����̏I��
	return 0;
}

/**************************************
*�@�摜�Ǎ�
***************************************/
int LoadImages(void)
{
	// �^�C�g���摜�̓Ǎ�
	if ((gTitleImg = LoadGraph("images/Title.bmp")) == -1) return -1;
	// ���j���[�摜�̓Ǎ�
	if ((gMenuImg = LoadGraph("images/menu.bmp")) == -1) return -1;
	// �J�[�\���摜�̓Ǎ�
	if ((gConeImg = LoadGraph("images/cone.bmp")) == -1) return -1;

	// �����L���O�摜�̓Ǎ�
	if ((gRankingImg = LoadGraph("images/Ranking.bmp")) == -1) return-1;

	// �A�C�e���摜�̓Ǎ�
	if ((gItemImg[0] = LoadGraph("images/gasoline.bmp")) == -1) return -1;
	if ((gItemImg[1] = LoadGraph("images/supana.bmp")) == -1) return -1;

	// �G���h�摜�̓Ǎ�
	if ((gEndImg = LoadGraph("images/End.bmp")) == -1) return -1;

	// �G�L�����摜�̕����Ǎ�
	if (LoadDivGraph("images/Car.bmp", 3, 3, 1, 63, 120, gEnemyImg) == -1) return -1;
	if ((gEnemyImg[3] = LoadGraph("images/gentuki.bmp")) == -1) return -1;

	// �X�e�[�W�w�i�摜�̓Ǎ�
	if ((gStageImg = LoadGraph("images/back.bmp")) == -1) return -1;

	// �L�����N�^�摜�̓Ǎ�
	if ((gCarImg = LoadGraph("images/car1pol.bmp")) == -1)return -1;
	if ((gBarrierImg = LoadGraph("images/barrier.png")) == -1)return -1;

	return 0;
}

/**************************************
*�@�^�C�g�����
***************************************/
void DrawTitle(void)
{
	static int menuNo = 0;

	// ���j���[�J�[�\���ړ�����
	if (gKeyFlg & PAD_INPUT_DOWN) {
		if (++menuNo > 3) menuNo = 0;
	}
	if (gKeyFlg & PAD_INPUT_UP) {
		if (--menuNo < 0) menuNo = 3;
	}

	// Z�L�[�Ń��j���[�I��
	if (gKeyFlg & PAD_INPUT_A) {
		switch (menuNo) {
		case 0:
			gGameMode = INIT;
			break;
		case 1:
			gGameMode = RANKING;
			break;
		case 2:
			gGameMode = HELP;
			break;
		case 3:
			gGameMode = END;
			break;
		}
	}

	// �^�C�g���A���j���[�A�J�[�\���摜�̕\��
	DrawGraph(0, 0, gTitleImg, 0);
	DrawGraph(120, 200, gMenuImg, 1);
	DrawRotaGraph(90, 220 + menuNo * 40, 0.7f, M_PI / 2, gConeImg, 1);

}
/**************************************
*�@�Q�[��������
***************************************/
void GameInit(void)
{
	// �X�R�A�̏�����
	gScore = 0;

	// ���s������������
	gMileage = 0;

	// �G����������̏�����
	gEnemyCount[0] = 0;
	gEnemyCount[1] = 0;
	gEnemyCount[2] = 0;

	// �v���C���[(���@)�̏����ݒ�
	gPlayer.flg = TRUE;
	gPlayer.x = PLAYER_POS_X;
	gPlayer.y = PLAYER_POS_Y;
	gPlayer.w = PLAYER_WIDTH;
	gPlayer.h = PLAYER_HEIGHT;
	gPlayer.angle = 0.0;
	gPlayer.count = 0;
	gPlayer.speed = PLAYER_SPEED;
	gPlayer.hp = PLAYER_HP;
	gPlayer.fuel = PLAYER_FUEL;
	gPlayer.bari = PLAYER_BARRIER;
	gPlayer.bariup = PLAYER_BARRIERUP;

	// �G�@�̏����ݒ�
	for (int i = 0; i < ENEMY_MAX; i++) {
		gEnemy[i].flg = FALSE;
	}

	// �A�C�e���̏����ݒ�
	for (int i = 0; i < ITEM_MAX; i++) {
		gItem[i].flg = FALSE;
	}

	// �Q�[�����C��������
	gGameMode = MAIN;
}

/**************************************
*�@�Q�[�����C��
***************************************/
void DrawMain(void)
{
	// ���H�X�N���[���֐�
	BackScrool();

	// �G�l�~�[����
	EnemyControl();

	// �A�C�e������
	ItemControl();

	// �v���C���[�̐���ƃQ�[����ԕ\��
	PlayerControl();

	// �X�y�[�X�L�[�Ń��j���[�ɖ߂�
	if (gKeyFlg & PAD_INPUT_M) gGameMode = GAMEOVER;

	SetFontSize(16);
	DrawString(20, 20, "�Q�[�����C��", 0xffffff, 0);
	DrawString(150, 450, "---- �X�y�[�X�L�[�������ăQ�[���I�[�o�[�� ----", 0xffffff, 0);
}

/**************************************
*�@�w���v���
***************************************/
void DrawHelp(void)
{
	// �X�y�[�X�L�[�Ń��j���[�ɖ߂�
	if (gKeyFlg & PAD_INPUT_M) gGameMode = TITLE;

	// �^�C�g���摜�\��
	DrawGraph(0, 0, gTitleImg, 0);

	SetFontSize(16);
	DrawString(20, 120, "�w���v���", 0xffffff, 0);

	DrawString(20, 160, "����͏�Q�����悯�Ȃ���", 0xffffff, 0);
	DrawString(20, 180, "���葱����Q�[���ł�", 0xffffff, 0);
	DrawString(20, 200, "�R�����s���邩��Q����", 0xffffff, 0);
	DrawString(20, 220, "���񓖂���ƃQ�[���I�[�o�[�ł�", 0xffffff, 0);
	DrawString(20, 250, "�A�C�e���ꗗ", 0xffffff, 0);
	DrawGraph(20, 260, gItemImg[0], 1);
	DrawString(20, 315, "���ƔR�����񕜂����", 0xffffff, 0);
	DrawGraph(20, 335, gItemImg[1], 1);
	DrawString(20, 385, "�_���[�W���󂯂Ă���Ƃ��Ɏ��Ƒϋv��", 0xffffff, 0);
	DrawString(20, 408, "�ϋv�������Ă��Ȃ�������R���������񕜂����", 0xffffff, 0);

	DrawString(150, 450, "--- �X�y�[�X�L�[�������ă^�C�g���֖߂� ---", 0xff0000, 0);
}

/**************************************
*�@�Q�[���I�[�o�[���
***************************************/
void DrawOver(void)
{
	// �X�R�A�̌v�Z����
	gScore = (gMileage / 10 * 10) + gEnemyCount[2] * 50 + gEnemyCount[1] * 100 + gEnemyCount[0] * 200;

	// �X�y�[�X�L�[�Ń��j���[�ɖ߂�
	if (gKeyFlg & PAD_INPUT_M) {
		if (gRanking[RANKING_DATA - 1].score >= gScore) {
			gGameMode = TITLE;
		}
		else {
			gGameMode = INPUTNAME;
		}
	}

	BackScrool();

	DrawBox(150, 150, 490, 330, 0x009900, 1);
	DrawBox(150, 150, 490, 330, 0x000000, 0);

	SetFontSize(20);
	DrawString(220, 170, "�Q�[���I�[�o�[", 0xcc0000);
	SetFontSize(16);
	DrawString(180, 200, "���s����       ", 0x000000);

	DrawRotaGraph(230, 230, 0.3f, M_PI / 2, gEnemyImg[2], 1, 0);
	DrawRotaGraph(230, 250, 0.3f, M_PI / 2, gEnemyImg[1], 1, 0);
	DrawRotaGraph(230, 270, 0.3f, M_PI / 2, gEnemyImg[0], 1, 0);

	DrawFormatString(260, 200, 0xFFFFFF, "%6d �~  10 = %6d", gMileage / 10, gMileage / 10 * 10);
	DrawFormatString(260, 222, 0xFFFFFF, "%6d �~  50 = %6d", gEnemyCount[2], gEnemyCount[2] * 50);
	DrawFormatString(260, 243, 0xFFFFFF, "%6d �~ 100 = %6d", gEnemyCount[1], gEnemyCount[1] * 100);
	DrawFormatString(260, 260, 0xFFFFFF, "%6d �~ 200 = %6d", gEnemyCount[0], gEnemyCount[0] * 200);

	DrawString(310, 290, "�X�R�A ", 0x000000);
	DrawFormatString(260, 290, 0xFFFFFF, "              = %6d", gScore);

	DrawString(150, 450, "---- �X�y�[�X�L�[�������ă^�C�g���֖߂� ----", 0xffffff, 0);
}

/**************************************
*�@�G���h���
***************************************/
void DrawEnd(void)
{
	// �G���h�摜�\��
	DrawGraph(0, 0, gEndImg, 0);

	SetFontSize(24);
	DrawString(360, 480 - 24, "Thank you for Playing", 0xffffff, 0);

	// �^�C���̉��Z����&�I��
	if (++gWaitTime > 180) gGameMode = CLOSE;
}

/**************************************
*�@�����L���O���
***************************************/
void DrawRanking(void)
{
	// �X�y�[�X�L�[�Ń��j���[�ɖ߂�
	if (gKeyFlg & PAD_INPUT_M) gGameMode = TITLE;

	// �����L���O�摜�\��
	DrawGraph(0, 0, gRankingImg, 0);

	// �����L���O�ꗗ��\��
	SetFontSize(18);
	for (int i = 0; i < RANKING_DATA; i++) {
		DrawFormatString(50, 170 + i * 25, 0xffffff, "%2d %-10s %10d", gRanking[i].no, gRanking[i].name, gRanking[i].score);
	}
	
	DrawString(100, 450, "--- �X�y�[�X�L�[�������ă^�C�g���֖߂� ---", 0xff0000, 0);
	
}
/**************************************
*�@�����L���O���͉��
***************************************/
void InputName(void)
{
	// �����L���O�摜�\��
	DrawGraph(0, 0, gTitleImg, 0);

	// �t�H���g�T�C�Y�̐ݒ�
	SetFontSize(20);

	// ���O���͎x��������̕`��
	DrawString(150, 240, "�����L���O�ɓo�^���܂�", 0xFFFFFF);
	DrawString(150, 270, "���O���p���œ��͂��Ă�������", 0xFFFFFF);

	// ���O�̓���
	DrawString(150, 310, "> ", 0xFFFFFF);
	DrawBox(160, 305, 300, 335, 0x000055, TRUE);
	if (KeyInputSingleCharString(170, 310, 10, gRanking[RANKING_DATA - 1].name, 0) == 1) {
		gRanking[RANKING_DATA - 1].score = gScore; // �����L���O�f�[�^�ɃX�R�A��o�^
		SortRanking();                             // �����L���O���בւ�
		SaveRanking();                             // �����L���O�f�[�^�̕ۑ�
		gGameMode = RANKING;                       // �Q�[�����[�h�̕ύX
	}
}
/***********************************************
 * �����L���O���בւ�
 ***********************************************/
void SortRanking(void)
{
	int i, j;
	RankingData work;

	// �I��@�\�[�g
	for (i = 0; i < RANKING_DATA - 1; i++) {
		for (j = i + 1; j < 10; j++) {
			if (gRanking[i].score <= gRanking[j].score) {
				work = gRanking[i];
				gRanking[i] = gRanking[j];
				gRanking[j] = work;
			}
		}
	}

	// ���ʕt��
	for (i = 0; i < RANKING_DATA; i++) {
		gRanking[i].no = 1;
	}
	// ���_�������ꍇ�́A�������ʂƂ���
	// �����ʂ��������ꍇ�̎��̏��ʂ̓f�[�^�������Z���ꂽ���ʂƂ���
	for (i = 0; i < RANKING_DATA - 1; i++) {
		for (j = i + 1; j < RANKING_DATA; j++) {
			if (gRanking[i].score > gRanking[j].score) {
				gRanking[j].no++;
			}
		}
	}
}

/***********************************************
 * �����L���O�f�[�^�̕ۑ�
 ***********************************************/
int  SaveRanking(void)
{
	FILE* fp;
#pragma warning(disable:4996)

	// �t�@�C���I�[�v��
	if ((fp = fopen("dat/rankingdata.txt", "w")) == NULL) {
		/* �G���[���� */
		printf("Ranking Data Error\n");
		return -1;
	}

	// �����L���O�f�[�^���z��f�[�^����������
	for (int i = 0; i < RANKING_DATA; i++) {
		fprintf(fp, "%2d %10s %10d\n", gRanking[i].no, gRanking[i].name, gRanking[i].score);
	}

	//�t�@�C���N���[�Y
	fclose(fp);

	return 0;

}

/*************************************
 * �����L���O�f�[�^�ǂݍ���
 *************************************/
int ReadRanking(void)
{
	FILE* fp;
#pragma warning(disable:4996)

	//�t�@�C���I�[�v��
	if ((fp = fopen("dat/rankingdata.txt", "r")) == NULL) {
		//�G���[����
		printf("Ranking Data Error\n");
		return -1;
	}

	//�����L���O�f�[�^�z����f�[�^��ǂݍ���
	for (int i = 0; i < RANKING_DATA; i++) {
		int dammy = fscanf(fp, "%2d %10s %10d", &gRanking[i].no, gRanking[i].name, &gRanking[i].score);
	}

	//�t�@�C���N���[�Y
	fclose(fp);

	return 0;
}
/*************************************
 * ���H�X�N���[������
 *************************************/
void BackScrool(void)
{
	// �X�e�[�W�摜�\��
	// �`��\�G���A��ݒ�
	SetDrawArea(0, 0, 500, 480);

	// �㕔�̓��H�W��
	DrawGraph(0, gMileage % 480 - 480, gStageImg, 0);
	// �����̓��H�W��
	DrawGraph(0, gMileage % 480, gStageImg, 0);

	// �G���A��߂�
	SetDrawArea(0, 0, 640, 480);

	// �X�R�A���\���̈�
	DrawBox(500, 0, 640, 480, 0x009900, 1);
}
/*************************************
 * �v���C���[�̈ړ�
 *************************************/
void PlayerControl(void)
{
	// ���s���������Z����
	gMileage += gPlayer.speed;

	// �R���̏���
	gPlayer.fuel -= gPlayer.speed;
	//gPlayer.fuel = PLAYER_FUEL;

	// �Q�[���I�[�o�[������
	if (gPlayer.fuel <= 0)gGameMode = GAMEOVER;

	// Z�L�[�ŉ���
	if (gKeyFlg & PAD_INPUT_A && gPlayer.speed < 10)gPlayer.speed += 1;

	// X�L�[�Ō���
	if (gKeyFlg & PAD_INPUT_B && gPlayer.speed > 1)gPlayer.speed -= 1;

	// �㉺���E�ړ�
	if (gPlayer.flg == TRUE) {
		if (gNowKey & PAD_INPUT_UP)gPlayer.y -= gPlayer.speed;
		if (gNowKey & PAD_INPUT_DOWN)gPlayer.y += gPlayer.speed;
		if (gNowKey & PAD_INPUT_LEFT)gPlayer.x -= gPlayer.speed;
		if (gNowKey & PAD_INPUT_RIGHT)gPlayer.x += gPlayer.speed;
	}

	// ��ʂ��͂ݏo���Ȃ��悤�ɂ���
	if (gPlayer.x < 32) gPlayer.x = 32;
	if (gPlayer.x > SCREEN_WIDTH - 180) gPlayer.x = SCREEN_WIDTH - 180;
	if (gPlayer.y < 60) gPlayer.y = 60;
	if (gPlayer.y > SCREEN_HEIGHT - 60) gPlayer.y = SCREEN_HEIGHT - 60;

	// �v���C���[�̕\��
	if (gPlayer.flg == TRUE) {
		// ���ړ�
		if (gNowKey & PAD_INPUT_LEFT) {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, -M_PI / 18, gCarImg, 1, 0);
		}
		// �E�ړ�
		else if (gNowKey & PAD_INPUT_RIGHT) {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, M_PI / 18, gCarImg, 1, 0);
		}
		// ���̑�
		else {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, 0, gCarImg, 1, 0);
		}

		// �o���A���g�p����
		if (gKeyFlg & PAD_INPUT_C && gPlayer.bari > 0 && gPlayer.baricnt <= 0) {
			gPlayer.bari--;
			gPlayer.baricnt = 2000;
		}

		// �o���A�g�p��
		if (gPlayer.baricnt > 0) {
			gPlayer.baricnt -= gPlayer.speed;
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, 0, gBarrierImg, 1, 0);
		}
		else {
			gPlayer.baricnt = 0;
		}
	}
	else {
		// �Փˎ�
		DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, M_PI / 8 * (++gPlayer.count / 5), gCarImg, 1, 0);
		if (gPlayer.count >= 80) gPlayer.flg = TRUE;
	}
	
	// �G�����������\��
	SetFontSize(16);
	DrawFormatString(510, 20, 0x000000, "�n�C�X�R�A");
	DrawFormatString(560, 40, 0xffffff, "%08d", gRanking[0].score);
	DrawFormatString(510, 80, 0x000000, "��������");

	DrawRotaGraph(523, 120, 0.3f, 0, gEnemyImg[0], 1, 0);
	DrawRotaGraph(573, 120, 0.3f, 0, gEnemyImg[1], 1, 0);
	DrawRotaGraph(623, 120, 0.3f, 0, gEnemyImg[2], 1, 0);

	DrawFormatString(510, 140, 0xffffff, "%03d", gEnemyCount[0]);
	DrawFormatString(560, 140, 0xffffff, "%03d", gEnemyCount[1]);
	DrawFormatString(610, 140, 0xffffff, "%03d", gEnemyCount[2]);

	DrawFormatString(510, 200, 0x000000, "���s����");
	DrawFormatString(555, 220, 0xffffff, "%08d", gMileage / 10);

	DrawFormatString(510, 240, 0x000000, "�X�s�[�h");
	DrawFormatString(555, 260, 0xffffff, "%08d", gPlayer.speed);

	// �o���A�̕\��
	for (int i = 0; i < gPlayer.bari; i++) {
		DrawRotaGraph(520 + i * 25, 340, 0.2f, 0, gBarrierImg, 1, 0);
	}

	// �R���Q�[�W�̕\��
	int F_X = 510; int F_Y = 390; int F_W = 100; int F_H = 20;
	DrawString(F_X, F_Y, "FUEL METER", 0x000000, 0);

	// �R���Q�[�W(����)
	DrawBox(F_X, F_Y + 20, F_X + (int)(gPlayer.fuel * F_W / PLAYER_FUEL), F_Y + 20 + F_H, 0x0066cc, 1);
	// �R���Q�[�W(�O��)
	DrawBox(F_X, F_Y + 20, F_X + F_W, F_Y + 20 + F_H, 0x000000, 0);

	// �̗̓Q�[�W�̕\��
	int X = 510; int Y = 430; int W = 100; int H = 20;

	DrawString(X, Y, "PLAYER HP", 0x000000, 0);
	// �̗̓Q�[�W(����)
	DrawBox(X, Y + 20, X + (int)(gPlayer.hp * W / PLAYER_HP), Y + 20 + H, 0xff0000, 1);
	// �̗̓Q�[�W(�O��)
	DrawBox(X, Y + 20, X + W, Y + 20 + H, 0x000000, 0);
}
/*************************************
 * �G�l�~�[�̈ړ�
 *************************************/
void EnemyControl()
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (gEnemy[i].flg == TRUE) {

			// �G�̕\��
			DrawRotaGraph(gEnemy[i].x, gEnemy[i].y, 1.0f, 0, gEnemy[i].img, 1, 0);

			if (gPlayer.flg == FALSE) continue;

			// �^���������Ɉړ�
			gEnemy[i].y += (gEnemy[i].speed + gPlayer.speed - PLAYER_SPEED + 1);

			// ���t�̂ݎ֍s����
			if (gEnemy[i].type == 3)
			{
				// �E�[�̏ꍇ���Ɏ֍s����
				if (gEnemy[i].x >= 460) {
					gEnemy[i].dako = 105;
				}
				if (gEnemy[i].d_flg == 1) {
					gEnemy[i].x++;
					if (++gEnemy[i].dako >= 105) {
						gEnemy[i].d_flg = 0;
					}
				}else{
					gEnemy[i].x--;
					if (--gEnemy[i].dako <= 0) {
						gEnemy[i].d_flg = 1;
					}
				}
			}

			// ��ʂ��͂ݏo���������
			if (gEnemy[i].y > SCREEN_HEIGHT + gEnemy[i].h)
				gEnemy[i].flg = FALSE;

			// �G�@��ǂ��z������J�E���g����
			if (gEnemy[i].y > gPlayer.y && gEnemy[i].point == 1) {
				gEnemy[i].point = 0;
				if (gEnemy[i].type == 0) gEnemyCount[0]++;
				if (gEnemy[i].type == 1) gEnemyCount[1]++;
				if (gEnemy[i].type == 2) gEnemyCount[2]++;
			}
			// �����蔻��
			if (HitBoxPlayer(&gPlayer, &gEnemy[i]) == TRUE && gPlayer.baricnt <= 0) {
				gPlayer.flg = FALSE;
				gPlayer.count = 0;
				gPlayer.hp -= 40;
				//gPlayer.hp = PLAYER_HP;
				gEnemy[i].flg = FALSE;
			    if (gPlayer.hp <= 0) gGameMode = GAMEOVER;
			}
		}
	}

	// ���s�������ƂɓG�o���p�^�[���𐧌䂷��
	if (gMileage / 10 % 50 == 0) {
		CreateEnemy();
	}
}

/*************************************
 * �G�l�~�[�̐���
 *************************************/
int CreateEnemy()
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (gEnemy[i].flg == FALSE) {
			gEnemy[i] = gEnemy00;
			gEnemy[i].type = GetRand(3);
			gEnemy[i].img = gEnemyImg[gEnemy[i].type];
			gEnemy[i].x = GetRand(4) * 105 + 40;
			gEnemy[i].speed = gEnemy[i].type * 1;
			// ����
			return TRUE;
		}
	}

	// ���s
	return FALSE;
}
/*************************************
 * ���@�ƓG�@�̓����蔻��(��`)
 *************************************/
int HitBoxPlayer(PLAYER* p, ENEMY* e)
{
	// x, y �͒��S���W�Ƃ���
	int sx1 = p->x - (p->w / 2);
	int sy1 = p->y - (p->h / 2);
	int sx2 = sx1 + p->w;
	int sy2 = sy1 + p->h;

	int dx1 = e->x - (e->w / 2);
	int dy1 = e->y - (e->h / 2);
	int dx2 = dx1 + e->w;
	int dy2 = dy1 + e->h;

	// ��`���d�Ȃ��Ă���Γ�����
	if (sx1 < dx2 && dx1 < sx2 && sy1 < dy2 && dy1 < sy2) {
		return TRUE;
	}
	return FALSE;
}
/*************************************
 * �A�C�e������
 *************************************/
void ItemControl()
{
	for (int i = 0; i < ITEM_MAX; i++) {
		if (gItem[i].flg == TRUE) {

			// �A�C�e���̕\��
			DrawRotaGraph(gItem[i].x, gItem[i].y, 1.0f, 0, gItem[i].img, 1, 0);

			if (gPlayer.flg == FALSE) continue;

			// �^���������Ɉړ�
			gItem[i].y += gItem[i].speed + gPlayer.speed - PLAYER_SPEED;

			// ��ʂ��͂ݏo���������
			if (gItem[i].y > SCREEN_HEIGHT) gItem[i].flg = FALSE;

			// �����蔻��
			if (HitBoxPlayer(&gPlayer, &gItem[i]) == TRUE) {
				gItem[i].flg = FALSE;
				// �����A�C�e��
				if (gItem[i].type == 0) {
					gPlayer.fuel += gItem[i].point;
					if (gPlayer.fuel > PLAYER_FUEL) gPlayer.fuel = PLAYER_FUEL;
				}

				// �H��A�C�e��
				if (gItem[i].type == 1) {
					gPlayer.hp += gItem[i].point;
					if (gPlayer.hp > PLAYER_HP) gPlayer.hp = PLAYER_HP;
				}
			}
		}
	}

	// ���s�������ƂɓG�o���p�^�[���𐧌䂷��
	if (gMileage / 10 % 500 == 0) {
		CreateItem();
	}
}
/*************************************
 * �A�C�e���̐���
 *************************************/
int CreateItem()
{
	for (int i = 0; i < ITEM_MAX; i++) {
		if (gItem[i].flg == FALSE) {
			gItem[i] = gItem00;
			gItem[i].type = GetRand(1);
			gItem[i].img = gItemImg[gItem[i].type];
			gItem[i].x = GetRand(4) * 105 + 40;
			gItem[i].speed = 1 + gItem[i].type * 3;
			if (gItem[i].type == 0) gItem[i].point = 5000;
			if (gItem[i].type == 1) gItem[i].point = 500;

			// ����
			return TRUE;
		}
	}

	// ���s
	return FALSE;
}