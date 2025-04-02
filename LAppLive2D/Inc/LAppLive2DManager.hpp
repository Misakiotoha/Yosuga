/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Type/csmVector.hpp>
#include <string>

class LAppModel;

/**
* @brief サンプルアプリケーションにおいてCubismModelを管理するクラス<br>
*         モデル生成と破棄、タップイベントの処理、モデル切り替えを行う。
*
*/
class LAppLive2DManager
{

public:
    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static LAppLive2DManager* GetInstance();

    /**
    * @brief   クラスのインスタンス（シングルトン）を解放する。
    *
    */
    static void ReleaseInstance();

    /**
    * @brief   Resources フォルダにあるモデルフォルダ名をセットする
    *
    * 
    * 无需多言，这个也宣布弃用了，这个函数功能是自动扫描路径下所有模型
    */
    void SetUpModel();

    /**
    * @brief   Resources フォルダにあるモデルフォルダ名を取得する
    *
    */
    Csm::csmVector<Csm::csmString> GetModelDir() const;

    /**
    * @brief   Resources フォルダにあるモデルフォルダのサイズを取得する
    *
    */
    Csm::csmInt32 GetModelDirSize() const;

    /**
    * @brief   現在のシーンで保持しているモデルを返す
    *
    * @param[in]   no  モデルリストのインデックス値
    * @return      モデルのインスタンスを返す。インデックス値が範囲外の場合はNULLを返す。
    */
    LAppModel* GetModel(Csm::csmUint32 no) const;

    /**
    * @brief   現在のシーンで保持しているすべてのモデルを解放する
    *
    */
    void ReleaseAllModel();

    /**
    * @brief   画面をドラッグしたときの処理
    *
    * @param[in]   x   画面のX座標
    * @param[in]   y   画面のY座標
    */
    void OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y) const;

    /**
    * @brief   画面をタップしたときの処理
    *
    * @param[in]   x   画面のX座標
    * @param[in]   y   画面のY座標
    */
    void OnTap(Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
    * @brief   画面を更新するときの処理
    *          モデルの更新処理および描画処理を行う
    */
    void OnUpdate() const;

    /**
     * @brief 从指定路径加载模型
     *
     * @param[in] modelPath 模型文件的路径
     * @param[in] fileName 模型文件的名称
     * 
     * Misaki 增设于2024.12.17 <br>
     * 
     * 举个例子：<br>
     *  LoadModelFromPath("Resources/Haru/", "Haru.model3.json");<br>
     * 
     * 至于为什么要这样拆分，只是为了适应底层的模型加载函数<br>
     * 你可以选择再上层封装，将传入的路径拆分为路径和文件名，然后调用本函数即可<br>
     */
    void LoadModelFromPath(const std::string& modelPath, const std::string& fileName);

    /**
    * @brief   次のシーンに切り替える<br>
    *           サンプルアプリケーションではモデルセットの切り替えを行う。
    */
    void NextScene();

    /**
    * @brief   シーンを切り替える<br>
    *           サンプルアプリケーションではモデルセットの切り替えを行う。
    * 
    * 如果你看到了这段文字，就代表你用的是被Misaki修改后的版本
    * 那么现在告诉你，这个函数已经弃用，请使用LoadModelFromPath来进行导入模型文件
    * 以及自行设计模型选择器
    * 至于为什么修改，原生的SDK是自动查找指定目录的模型文件，自动加载的
    * 但这样就与实际使用的需求大不相同，我们需要能够手动选择
    */
    void ChangeScene(Csm::csmInt32 index);

    /**
     * @brief 启动当前活动模型的唇形同步并播放指定的 WAV 文件
     * @param wavFilePath WAV 文件的路径
     */
    void StartLipSync(const Csm::csmString& wavFilePath);

    /**
     * @brief 启动指定模型的唇形同步并播放指定的 WAV 文件
     * @param modelIndex 模型索引
     * @param wavFilePath WAV 文件的路径
     */
    void StartLipSync(Live2D::Cubism::Framework::csmUint32 modelIndex, const Csm::csmString& wavFilePath);

    /**
     * @brief   モデル個数を得る
     * @return  所持モデル個数
     */
    Csm::csmUint32 GetModelNum() const;

    /**
     * @brief   viewMatrixをセットする
     */
    void SetViewMatrix(Live2D::Cubism::Framework::CubismMatrix44* m);

private:
    /**
    * @brief  コンストラクタ
    */
    LAppLive2DManager();

    /**
    * @brief  デストラクタ
    */
    virtual ~LAppLive2DManager();

    Csm::CubismMatrix44* _viewMatrix; ///< モデル描画に用いるView行列
    Csm::csmVector<LAppModel*> _models; ///< モデルインスタンスのコンテナ
    Csm::csmInt32 _sceneIndex; ///< 表示するシーンのインデックス値

    Csm::csmVector<Csm::csmString> _modelDir; ///< モデルディレクトリ名のコンテナ
};
