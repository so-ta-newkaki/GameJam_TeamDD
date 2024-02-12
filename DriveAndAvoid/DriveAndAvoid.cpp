/**************************************
**　第5章　ミニゲームを作る(2)
**　カーレース&避けるゲーム
**　2022年8月
***************************************/
#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define RANKING_DATA 5

/**************************************
*　列挙型の宣言
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
*　変数の宣言
***************************************/
// 画面領域の大きさ
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// 自機の初期値の定数
const int PLAYER_POS_X = SCREEN_WIDTH / 2;
const int PLAYER_POS_Y = SCREEN_HEIGHT - 100;
const int PLAYER_WIDTH = 20;//63
const int PLAYER_HEIGHT = 60;//120
const int PLAYER_SPEED = 5;
const int PLAYER_HP = 1000;
const int PLAYER_FUEL = 20000;
const int PLAYER_BARRIER = 3;
const int PLAYER_BARRIERUP = 10;

// 敵機の最大数
const int ENEMY_MAX = 8;

// アイテムの最大値
const int ITEM_MAX = 3;

/**************************************
*　変数の宣言(グローバル変数)
***************************************/
int gOldKey;           // 前回の入力キー
int gNowKey;           // 今回の入力キー
int gKeyFlg;           // 入力キー情報

int gGameMode = TITLE;    // ゲームモード

int gTitleImg;         // タイトル画像
int gMenuImg;          // メニュー画像
int gConeImg;          // メニューカーソル画像

int gScore = 0;        // スコア
int gRankingImg;       // ランキング画面背景

int gItemImg[2];       // アイテム画像

int gWaitTime = 0;      // 待ち時間
int gEndImg;            // エンド画像

int gMileage;          // 走行距離
int gEnemyCount[4];    // 敵カウント
int gEnemyImg[4];      // 敵キャラ画像

int gStageImg;         // ステージ画像(道路)

int gCarImg;           // 車の画像
int gBarrierImg;       // バリアの画像

/**************************************
*　構造体
***************************************/
// ランキングデータ構造体
struct RankingData {
	int     no;
	char    name[11];
	long    score;

};

// ランキングデータの変数宣言
struct RankingData    gRanking[RANKING_DATA];

// 自機の構造体
struct PLAYER {
	int flg;          // 使用フラグ
	int x, y;         // 座標
	int w, h;         // 幅・高さ
	double angle;     // 機体の向き
	int count;        // タイミング用
	int speed;        // 移動速度
	int hp;           // 体力
	int fuel;         // 燃料
	int bari;         // バリア回数
	int baricnt;      // バリア継続時間
	int bariup;       // バリア回数(走行距離アップ)
};

// 自機の変数宣言
struct PLAYER gPlayer;

// 敵機の構造体
struct ENEMY {
	int flg;        // 使用フラグ
	int type;       // タイプ
	int img;        // 画像
	int x, y, w, h; // 座標、幅、高さ
	int speed;      // 移動速度
	int point;      // スコア加算ポイント
	int dako;       // 蛇行位置
	int d_flg;
};

// 敵機の変数宣言
struct ENEMY gEnemy[ENEMY_MAX];
struct ENEMY gEnemy00 = { TRUE,0,0,0,-50,63,120,0,1 };
struct ENEMY gEnemyCn = { TRUE,4,0,0,-50,18,18,0,1 };

// アイテムの変数宣言
struct ENEMY gItem[ITEM_MAX];
struct ENEMY gItem00 = { TRUE,0,0,0,-50,50,50,0,1 };

/**************************************
*　関数のプロトタイプ宣言
***************************************/
int LoadImages(void);     // 画像読込処理
void GameInit(void);      // ゲーム初期化処理

void DrawTitle(void);     // タイトル描画処理
void DrawMain(void);      // ゲームメイン描画処理
void DrawHelp(void);      // ゲームヘルプ描画処理
void DrawRanking(void);   // ランキング描画処理
void DrawOver(void);      // ゲームオーバー描画処理
void DrawEnd(void);       // ゲームエンド描画処理

void InputName(void);     // ハイスコアの名前入力処理

void SortRanking(void);   // ランキングの並べ替え
int SaveRanking(void);    // ランキングデータの保存
int ReadRanking(void);    // ランキングデータの読込

void BackScrool(void);    // 背景画像のスクロール処理

void PlayerControl(void); // プレイヤー制御とゲーム状態表示

void EnemyControl(void);  // 敵機処理
int CreateEnemy(void);    // 敵機生成処理

int HitBoxPlayer(PLAYER* p, ENEMY* e); // 当たり判定

void ItemControl(void);   // アイテム処理
int CreateItem(void);     // アイテム生成処理

/**************************************
*　プログラムの開始
***************************************/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// タイトルを設定
	SetMainWindowText("Drive&Avoid");

	// ウィンドウモードで起動
	ChangeWindowMode(TRUE);

	// DXライブラリの初期化処理
	if (DxLib_Init() == -1)return -1;

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像読込関数の呼び出し
	if (LoadImages() == -1) return -1;

	// ランキングデータの読込
	if (ReadRanking() == -1) return -1;

	// ゲームループ
	while (ProcessMessage() == 0 && gGameMode != CLOSE && !(gKeyFlg & PAD_INPUT_START)) {

		// 入力キー取得
		gOldKey = gNowKey;
		gNowKey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		gKeyFlg = gNowKey & ~gOldKey;

		// 画面の初期化
		ClearDrawScreen();

		//ゲームモードと画面遷移の制御
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

		// 裏画面の内容を表画面に反映する
		ScreenFlip();
	}

		// DXライブラリ使用の終了処理
		DxLib_End();
	
	// プログラムの終了
	return 0;
}

/**************************************
*　画像読込
***************************************/
int LoadImages(void)
{
	// タイトル画像の読込
	if ((gTitleImg = LoadGraph("images/Title.bmp")) == -1) return -1;
	// メニュー画像の読込
	if ((gMenuImg = LoadGraph("images/menu.bmp")) == -1) return -1;
	// カーソル画像の読込
	if ((gConeImg = LoadGraph("images/cone.bmp")) == -1) return -1;

	// ランキング画像の読込
	if ((gRankingImg = LoadGraph("images/Ranking.bmp")) == -1) return-1;

	// アイテム画像の読込
	if ((gItemImg[0] = LoadGraph("images/gasoline.bmp")) == -1) return -1;
	if ((gItemImg[1] = LoadGraph("images/supana.bmp")) == -1) return -1;

	// エンド画像の読込
	if ((gEndImg = LoadGraph("images/End.bmp")) == -1) return -1;

	// 敵キャラ画像の分割読込
	if (LoadDivGraph("images/Car.bmp", 3, 3, 1, 63, 120, gEnemyImg) == -1) return -1;
	if ((gEnemyImg[3] = LoadGraph("images/gentuki.bmp")) == -1) return -1;

	// ステージ背景画像の読込
	if ((gStageImg = LoadGraph("images/back.bmp")) == -1) return -1;

	// キャラクタ画像の読込
	if ((gCarImg = LoadGraph("images/car1pol.bmp")) == -1)return -1;
	if ((gBarrierImg = LoadGraph("images/barrier.png")) == -1)return -1;

	return 0;
}

/**************************************
*　タイトル画面
***************************************/
void DrawTitle(void)
{
	static int menuNo = 0;

	// メニューカーソル移動処理
	if (gKeyFlg & PAD_INPUT_DOWN) {
		if (++menuNo > 3) menuNo = 0;
	}
	if (gKeyFlg & PAD_INPUT_UP) {
		if (--menuNo < 0) menuNo = 3;
	}

	// Zキーでメニュー選択
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

	// タイトル、メニュー、カーソル画像の表示
	DrawGraph(0, 0, gTitleImg, 0);
	DrawGraph(120, 200, gMenuImg, 1);
	DrawRotaGraph(90, 220 + menuNo * 40, 0.7f, M_PI / 2, gConeImg, 1);

}
/**************************************
*　ゲーム初期化
***************************************/
void GameInit(void)
{
	// スコアの初期化
	gScore = 0;

	// 走行距離を初期化
	gMileage = 0;

	// 敵を避けた数の初期化
	gEnemyCount[0] = 0;
	gEnemyCount[1] = 0;
	gEnemyCount[2] = 0;

	// プレイヤー(自機)の初期設定
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

	// 敵機の初期設定
	for (int i = 0; i < ENEMY_MAX; i++) {
		gEnemy[i].flg = FALSE;
	}

	// アイテムの初期設定
	for (int i = 0; i < ITEM_MAX; i++) {
		gItem[i].flg = FALSE;
	}

	// ゲームメイン処理へ
	gGameMode = MAIN;
}

/**************************************
*　ゲームメイン
***************************************/
void DrawMain(void)
{
	// 道路スクロール関数
	BackScrool();

	// エネミー制御
	EnemyControl();

	// アイテム制御
	ItemControl();

	// プレイヤーの制御とゲーム状態表示
	PlayerControl();

	// スペースキーでメニューに戻る
	if (gKeyFlg & PAD_INPUT_M) gGameMode = GAMEOVER;

	SetFontSize(16);
	DrawString(20, 20, "ゲームメイン", 0xffffff, 0);
	DrawString(150, 450, "---- スペースキーを押してゲームオーバーへ ----", 0xffffff, 0);
}

/**************************************
*　ヘルプ画面
***************************************/
void DrawHelp(void)
{
	// スペースキーでメニューに戻る
	if (gKeyFlg & PAD_INPUT_M) gGameMode = TITLE;

	// タイトル画像表示
	DrawGraph(0, 0, gTitleImg, 0);

	SetFontSize(16);
	DrawString(20, 120, "ヘルプ画面", 0xffffff, 0);

	DrawString(20, 160, "これは障害物をよけながら", 0xffffff, 0);
	DrawString(20, 180, "走り続けるゲームです", 0xffffff, 0);
	DrawString(20, 200, "燃料が尽きるか障害物に", 0xffffff, 0);
	DrawString(20, 220, "数回当たるとゲームオーバーです", 0xffffff, 0);
	DrawString(20, 250, "アイテム一覧", 0xffffff, 0);
	DrawGraph(20, 260, gItemImg[0], 1);
	DrawString(20, 315, "取ると燃料が回復するよ", 0xffffff, 0);
	DrawGraph(20, 335, gItemImg[1], 1);
	DrawString(20, 385, "ダメージを受けているときに取ると耐久回復", 0xffffff, 0);
	DrawString(20, 408, "耐久が減っていなかったら燃料が少し回復するよ", 0xffffff, 0);

	DrawString(150, 450, "--- スペースキーを押してタイトルへ戻る ---", 0xff0000, 0);
}

/**************************************
*　ゲームオーバー画面
***************************************/
void DrawOver(void)
{
	// スコアの計算処理
	gScore = (gMileage / 10 * 10) + gEnemyCount[2] * 50 + gEnemyCount[1] * 100 + gEnemyCount[0] * 200;

	// スペースキーでメニューに戻る
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
	DrawString(220, 170, "ゲームオーバー", 0xcc0000);
	SetFontSize(16);
	DrawString(180, 200, "走行距離       ", 0x000000);

	DrawRotaGraph(230, 230, 0.3f, M_PI / 2, gEnemyImg[2], 1, 0);
	DrawRotaGraph(230, 250, 0.3f, M_PI / 2, gEnemyImg[1], 1, 0);
	DrawRotaGraph(230, 270, 0.3f, M_PI / 2, gEnemyImg[0], 1, 0);

	DrawFormatString(260, 200, 0xFFFFFF, "%6d ×  10 = %6d", gMileage / 10, gMileage / 10 * 10);
	DrawFormatString(260, 222, 0xFFFFFF, "%6d ×  50 = %6d", gEnemyCount[2], gEnemyCount[2] * 50);
	DrawFormatString(260, 243, 0xFFFFFF, "%6d × 100 = %6d", gEnemyCount[1], gEnemyCount[1] * 100);
	DrawFormatString(260, 260, 0xFFFFFF, "%6d × 200 = %6d", gEnemyCount[0], gEnemyCount[0] * 200);

	DrawString(310, 290, "スコア ", 0x000000);
	DrawFormatString(260, 290, 0xFFFFFF, "              = %6d", gScore);

	DrawString(150, 450, "---- スペースキーを押してタイトルへ戻る ----", 0xffffff, 0);
}

/**************************************
*　エンド画面
***************************************/
void DrawEnd(void)
{
	// エンド画像表示
	DrawGraph(0, 0, gEndImg, 0);

	SetFontSize(24);
	DrawString(360, 480 - 24, "Thank you for Playing", 0xffffff, 0);

	// タイムの加算処理&終了
	if (++gWaitTime > 180) gGameMode = CLOSE;
}

/**************************************
*　ランキング画面
***************************************/
void DrawRanking(void)
{
	// スペースキーでメニューに戻る
	if (gKeyFlg & PAD_INPUT_M) gGameMode = TITLE;

	// ランキング画像表示
	DrawGraph(0, 0, gRankingImg, 0);

	// ランキング一覧を表示
	SetFontSize(18);
	for (int i = 0; i < RANKING_DATA; i++) {
		DrawFormatString(50, 170 + i * 25, 0xffffff, "%2d %-10s %10d", gRanking[i].no, gRanking[i].name, gRanking[i].score);
	}
	
	DrawString(100, 450, "--- スペースキーを押してタイトルへ戻る ---", 0xff0000, 0);
	
}
/**************************************
*　ランキング入力画面
***************************************/
void InputName(void)
{
	// ランキング画像表示
	DrawGraph(0, 0, gTitleImg, 0);

	// フォントサイズの設定
	SetFontSize(20);

	// 名前入力支持文字列の描画
	DrawString(150, 240, "ランキングに登録します", 0xFFFFFF);
	DrawString(150, 270, "名前を英字で入力してください", 0xFFFFFF);

	// 名前の入力
	DrawString(150, 310, "> ", 0xFFFFFF);
	DrawBox(160, 305, 300, 335, 0x000055, TRUE);
	if (KeyInputSingleCharString(170, 310, 10, gRanking[RANKING_DATA - 1].name, 0) == 1) {
		gRanking[RANKING_DATA - 1].score = gScore; // ランキングデータにスコアを登録
		SortRanking();                             // ランキング並べ替え
		SaveRanking();                             // ランキングデータの保存
		gGameMode = RANKING;                       // ゲームモードの変更
	}
}
/***********************************************
 * ランキング並べ替え
 ***********************************************/
void SortRanking(void)
{
	int i, j;
	RankingData work;

	// 選択法ソート
	for (i = 0; i < RANKING_DATA - 1; i++) {
		for (j = i + 1; j < 10; j++) {
			if (gRanking[i].score <= gRanking[j].score) {
				work = gRanking[i];
				gRanking[i] = gRanking[j];
				gRanking[j] = work;
			}
		}
	}

	// 順位付け
	for (i = 0; i < RANKING_DATA; i++) {
		gRanking[i].no = 1;
	}
	// 得点が同じ場合は、同じ順位とする
	// 同順位があった場合の次の順位はデータ個数が加算された順位とする
	for (i = 0; i < RANKING_DATA - 1; i++) {
		for (j = i + 1; j < RANKING_DATA; j++) {
			if (gRanking[i].score > gRanking[j].score) {
				gRanking[j].no++;
			}
		}
	}
}

/***********************************************
 * ランキングデータの保存
 ***********************************************/
int  SaveRanking(void)
{
	FILE* fp;
#pragma warning(disable:4996)

	// ファイルオープン
	if ((fp = fopen("dat/rankingdata.txt", "w")) == NULL) {
		/* エラー処理 */
		printf("Ranking Data Error\n");
		return -1;
	}

	// ランキングデータ分配列データを書き込む
	for (int i = 0; i < RANKING_DATA; i++) {
		fprintf(fp, "%2d %10s %10d\n", gRanking[i].no, gRanking[i].name, gRanking[i].score);
	}

	//ファイルクローズ
	fclose(fp);

	return 0;

}

/*************************************
 * ランキングデータ読み込み
 *************************************/
int ReadRanking(void)
{
	FILE* fp;
#pragma warning(disable:4996)

	//ファイルオープン
	if ((fp = fopen("dat/rankingdata.txt", "r")) == NULL) {
		//エラー処理
		printf("Ranking Data Error\n");
		return -1;
	}

	//ランキングデータ配分列データを読み込む
	for (int i = 0; i < RANKING_DATA; i++) {
		int dammy = fscanf(fp, "%2d %10s %10d", &gRanking[i].no, gRanking[i].name, &gRanking[i].score);
	}

	//ファイルクローズ
	fclose(fp);

	return 0;
}
/*************************************
 * 道路スクロール処理
 *************************************/
void BackScrool(void)
{
	// ステージ画像表示
	// 描画可能エリアを設定
	SetDrawArea(0, 0, 500, 480);

	// 上部の道路標示
	DrawGraph(0, gMileage % 480 - 480, gStageImg, 0);
	// 下部の道路標示
	DrawGraph(0, gMileage % 480, gStageImg, 0);

	// エリアを戻す
	SetDrawArea(0, 0, 640, 480);

	// スコア等表示領域
	DrawBox(500, 0, 640, 480, 0x009900, 1);
}
/*************************************
 * プレイヤーの移動
 *************************************/
void PlayerControl(void)
{
	// 走行距離を加算する
	gMileage += gPlayer.speed;

	// 燃料の消費
	gPlayer.fuel -= gPlayer.speed;
	//gPlayer.fuel = PLAYER_FUEL;

	// ゲームオーバー処理へ
	if (gPlayer.fuel <= 0)gGameMode = GAMEOVER;

	// Zキーで加速
	if (gKeyFlg & PAD_INPUT_A && gPlayer.speed < 10)gPlayer.speed += 1;

	// Xキーで減速
	if (gKeyFlg & PAD_INPUT_B && gPlayer.speed > 1)gPlayer.speed -= 1;

	// 上下左右移動
	if (gPlayer.flg == TRUE) {
		if (gNowKey & PAD_INPUT_UP)gPlayer.y -= gPlayer.speed;
		if (gNowKey & PAD_INPUT_DOWN)gPlayer.y += gPlayer.speed;
		if (gNowKey & PAD_INPUT_LEFT)gPlayer.x -= gPlayer.speed;
		if (gNowKey & PAD_INPUT_RIGHT)gPlayer.x += gPlayer.speed;
	}

	// 画面をはみ出さないようにする
	if (gPlayer.x < 32) gPlayer.x = 32;
	if (gPlayer.x > SCREEN_WIDTH - 180) gPlayer.x = SCREEN_WIDTH - 180;
	if (gPlayer.y < 60) gPlayer.y = 60;
	if (gPlayer.y > SCREEN_HEIGHT - 60) gPlayer.y = SCREEN_HEIGHT - 60;

	// プレイヤーの表示
	if (gPlayer.flg == TRUE) {
		// 左移動
		if (gNowKey & PAD_INPUT_LEFT) {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, -M_PI / 18, gCarImg, 1, 0);
		}
		// 右移動
		else if (gNowKey & PAD_INPUT_RIGHT) {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, M_PI / 18, gCarImg, 1, 0);
		}
		// その他
		else {
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, 0, gCarImg, 1, 0);
		}

		// バリアを使用する
		if (gKeyFlg & PAD_INPUT_C && gPlayer.bari > 0 && gPlayer.baricnt <= 0) {
			gPlayer.bari--;
			gPlayer.baricnt = 2000;
		}

		// バリア使用中
		if (gPlayer.baricnt > 0) {
			gPlayer.baricnt -= gPlayer.speed;
			DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, 0, gBarrierImg, 1, 0);
		}
		else {
			gPlayer.baricnt = 0;
		}
	}
	else {
		// 衝突時
		DrawRotaGraph(gPlayer.x, gPlayer.y, 1.0f, M_PI / 8 * (++gPlayer.count / 5), gCarImg, 1, 0);
		if (gPlayer.count >= 80) gPlayer.flg = TRUE;
	}
	
	// 敵を避けた数を表示
	SetFontSize(16);
	DrawFormatString(510, 20, 0x000000, "ハイスコア");
	DrawFormatString(560, 40, 0xffffff, "%08d", gRanking[0].score);
	DrawFormatString(510, 80, 0x000000, "避けた数");

	DrawRotaGraph(523, 120, 0.3f, 0, gEnemyImg[0], 1, 0);
	DrawRotaGraph(573, 120, 0.3f, 0, gEnemyImg[1], 1, 0);
	DrawRotaGraph(623, 120, 0.3f, 0, gEnemyImg[2], 1, 0);

	DrawFormatString(510, 140, 0xffffff, "%03d", gEnemyCount[0]);
	DrawFormatString(560, 140, 0xffffff, "%03d", gEnemyCount[1]);
	DrawFormatString(610, 140, 0xffffff, "%03d", gEnemyCount[2]);

	DrawFormatString(510, 200, 0x000000, "走行距離");
	DrawFormatString(555, 220, 0xffffff, "%08d", gMileage / 10);

	DrawFormatString(510, 240, 0x000000, "スピード");
	DrawFormatString(555, 260, 0xffffff, "%08d", gPlayer.speed);

	// バリアの表示
	for (int i = 0; i < gPlayer.bari; i++) {
		DrawRotaGraph(520 + i * 25, 340, 0.2f, 0, gBarrierImg, 1, 0);
	}

	// 燃料ゲージの表示
	int F_X = 510; int F_Y = 390; int F_W = 100; int F_H = 20;
	DrawString(F_X, F_Y, "FUEL METER", 0x000000, 0);

	// 燃料ゲージ(内側)
	DrawBox(F_X, F_Y + 20, F_X + (int)(gPlayer.fuel * F_W / PLAYER_FUEL), F_Y + 20 + F_H, 0x0066cc, 1);
	// 燃料ゲージ(外部)
	DrawBox(F_X, F_Y + 20, F_X + F_W, F_Y + 20 + F_H, 0x000000, 0);

	// 体力ゲージの表示
	int X = 510; int Y = 430; int W = 100; int H = 20;

	DrawString(X, Y, "PLAYER HP", 0x000000, 0);
	// 体力ゲージ(内側)
	DrawBox(X, Y + 20, X + (int)(gPlayer.hp * W / PLAYER_HP), Y + 20 + H, 0xff0000, 1);
	// 体力ゲージ(外側)
	DrawBox(X, Y + 20, X + W, Y + 20 + H, 0x000000, 0);
}
/*************************************
 * エネミーの移動
 *************************************/
void EnemyControl()
{
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (gEnemy[i].flg == TRUE) {

			// 敵の表示
			DrawRotaGraph(gEnemy[i].x, gEnemy[i].y, 1.0f, 0, gEnemy[i].img, 1, 0);

			if (gPlayer.flg == FALSE) continue;

			// 真っ直ぐ下に移動
			gEnemy[i].y += (gEnemy[i].speed + gPlayer.speed - PLAYER_SPEED + 1);

			// 原付のみ蛇行処理
			if (gEnemy[i].type == 3)
			{
				// 右端の場合左に蛇行する
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

			// 画面をはみ出したら消去
			if (gEnemy[i].y > SCREEN_HEIGHT + gEnemy[i].h)
				gEnemy[i].flg = FALSE;

			// 敵機を追い越したらカウントする
			if (gEnemy[i].y > gPlayer.y && gEnemy[i].point == 1) {
				gEnemy[i].point = 0;
				if (gEnemy[i].type == 0) gEnemyCount[0]++;
				if (gEnemy[i].type == 1) gEnemyCount[1]++;
				if (gEnemy[i].type == 2) gEnemyCount[2]++;
			}
			// 当たり判定
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

	// 走行距離ごとに敵出現パターンを制御する
	if (gMileage / 10 % 50 == 0) {
		CreateEnemy();
	}
}

/*************************************
 * エネミーの生成
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
			// 成功
			return TRUE;
		}
	}

	// 失敗
	return FALSE;
}
/*************************************
 * 自機と敵機の当たり判定(矩形)
 *************************************/
int HitBoxPlayer(PLAYER* p, ENEMY* e)
{
	// x, y は中心座標とする
	int sx1 = p->x - (p->w / 2);
	int sy1 = p->y - (p->h / 2);
	int sx2 = sx1 + p->w;
	int sy2 = sy1 + p->h;

	int dx1 = e->x - (e->w / 2);
	int dy1 = e->y - (e->h / 2);
	int dx2 = dx1 + e->w;
	int dy2 = dy1 + e->h;

	// 矩形が重なっていれば当たり
	if (sx1 < dx2 && dx1 < sx2 && sy1 < dy2 && dy1 < sy2) {
		return TRUE;
	}
	return FALSE;
}
/*************************************
 * アイテム処理
 *************************************/
void ItemControl()
{
	for (int i = 0; i < ITEM_MAX; i++) {
		if (gItem[i].flg == TRUE) {

			// アイテムの表示
			DrawRotaGraph(gItem[i].x, gItem[i].y, 1.0f, 0, gItem[i].img, 1, 0);

			if (gPlayer.flg == FALSE) continue;

			// 真っ直ぐ下に移動
			gItem[i].y += gItem[i].speed + gPlayer.speed - PLAYER_SPEED;

			// 画面をはみ出したら消去
			if (gItem[i].y > SCREEN_HEIGHT) gItem[i].flg = FALSE;

			// 当たり判定
			if (HitBoxPlayer(&gPlayer, &gItem[i]) == TRUE) {
				gItem[i].flg = FALSE;
				// 給油アイテム
				if (gItem[i].type == 0) {
					gPlayer.fuel += gItem[i].point;
					if (gPlayer.fuel > PLAYER_FUEL) gPlayer.fuel = PLAYER_FUEL;
				}

				// 工具アイテム
				if (gItem[i].type == 1) {
					gPlayer.hp += gItem[i].point;
					if (gPlayer.hp > PLAYER_HP) gPlayer.hp = PLAYER_HP;
				}
			}
		}
	}

	// 走行距離ごとに敵出現パターンを制御する
	if (gMileage / 10 % 500 == 0) {
		CreateItem();
	}
}
/*************************************
 * アイテムの生成
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

			// 成功
			return TRUE;
		}
	}

	// 失敗
	return FALSE;
}