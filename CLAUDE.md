# roBa ZMK Config - Claude Code Guide

## リポジトリ概要

roBa 分割キーボードの ZMK ファームウェア設定。右半分にトラックボール（PMW3610）搭載。

## ブランチ運用

| ブランチ | 役割 |
|---|---|
| `main` | mac 用の設定。mac の操作感を基準に維持する。 |
| `windows-pc` | Windows 用の設定。Windows の操作感を基準に維持する。 |

### 分離ルール

- `main` と `windows-pc` は同期しない。
- 片方での変更をもう片方へ自動反映しない。
- それぞれ独立した完成形として扱う。
- 共通化したくなる変更も、必要なら各ブランチで別々に実装する。

## 主要ファイル

| ファイル | 役割 |
|---|---|
| `config/roBa.keymap` | キーマップ・ジェスチャー設定（メイン） |
| `config/west.yml` | ZMK 依存関係 |
| `boards/shields/roBa/roBa_R.overlay` | 右半分ハードウェア設定・input-processors 登録 |
| `zephyr/module.yml` | ローカルモジュール（zmk-mouse-gesture-multi）の登録 |
| `zmk-mouse-gesture-multi/` | マウスジェスチャーのローカルフォーク（独自改造済み） |

## ブランチ運用

- メインブランチ: `main`
- Windows 用の作業ブランチ: `windows-pc`
- 変更は必ず対象ブランチで完結させる
- `main` と `windows-pc` の間で機能同期はしない
- 片方の変更をもう片方へ持ち込まない

## レイヤー構成

| 番号 | 名前 | 呼び出し方 |
|---|---|---|
| 0 | default_layer | 常時 |
| 1 | FUNCTION | 右スペース長押し |
| 2 | NUM | 左バックスペース長押し / 左スペース長押し |
| 3 | CURSOR | セミコロン長押し |
| 4 | MOUSE | トラックボール移動で自動 (automouse) |
| 5 | SCROLL | scroll-layers 設定 |
| 6 | GESTURE | 未使用（予約） |
| 7 | layer_7 | LANG2 長押し（Bluetooth 設定等） |

## マウスジェスチャー設定

### 仕組み

- トラックボール搭載キー（I / O / P）を長押し（300ms）しながらトラックボールを動かすとジェスチャーが発動
- `&mouse_gesture_kp INSTANCE_ID KEY` の形式でキーマップに記述
- インスタンスごとに別々のジェスチャーを設定できる（独自改造済みモジュール）
- **重要:** デフォルトレイヤーと MOUSE レイヤー（layer 4）の両方に同じバインディングを設定すること（MOUSE レイヤーに設定しないとトラックボール移動中にジェスチャーが無効になる）

### インスタンス一覧

| instance-id | キー | 宣言ノード |
|---|---|---|
| 0 | O ボタン | `zip_mouse_gesture_0` |
| 1 | I ボタン | `zip_mouse_gesture_1` |
| 2 | P ボタン | `zip_mouse_gesture_2` |

### 現在のジェスチャー割り当て

**O ボタン (instance 0)**
| 方向 | アクション |
|---|---|
| 左 | Ctrl+← (仮想デスクトップ左) |
| 右 | Ctrl+→ (仮想デスクトップ右) |
| 上 | Ctrl+↑ (Mission Control) |
| 下 | Cmd+W (タブを閉じる) |

**I ボタン (instance 1)**
| 方向 | アクション |
|---|---|
| 左 | Ctrl+Shift+Tab (前のタブ) |
| 右 | Ctrl+Tab (次のタブ) |
| 上 | Cmd+Shift+T (閉じたタブを再度開く) |
| 下 | Cmd+W (タブを閉じる) |

**P ボタン (instance 2)**
| 方向 | アクション |
|---|---|
| 左 | Cmd+← |
| 上 | Cmd+Option+F |
| 右 | Cmd+→ |
| 左→下 | Cmd+1 |
| 右→下 | Cmd+4 |
| 下→左 | Cmd+2 |
| 下→右 | Cmd+3 |

### 新しいジェスチャーインスタンスを追加する手順

1. `config/roBa.keymap` にジェスチャーノードを追加:
```dts
/ {
    zip_mouse_gesture_N: zip_mouse_gesture_N {
        compatible = "zmk,input-processor-mouse-gesture";
        #input-processor-cells = <0>;
        instance-id = <N>;
        enable-eager-mode;

        gesture_name {
            pattern = <GESTURE_LEFT>;       // GESTURE_LEFT/RIGHT/UP/DOWN
            bindings = <&kp XXX>;
        };
        // 複数ステップ例: 左→下
        multi_step {
            pattern = <GESTURE_LEFT GESTURE_DOWN>;
            bindings = <&kp XXX>;
        };
    };
};
```

2. `boards/shields/roBa/roBa_R.overlay` の input-processors に追加:
```dts
input-processors = <&zip_mouse_gesture_0>, <&zip_mouse_gesture_1>, <&zip_mouse_gesture_2>, <&zip_mouse_gesture_N>;
```

3. デフォルトレイヤーと MOUSE レイヤーのキーを `&mouse_gesture_kp N KEY` に変更

## デフォルトレイヤー キーマップ

```
Q    W    E    R    T    |    Y    U(hold=_)  I(gesture1)  O(gesture0)  P(gesture2)
A    S    D    F    G    |   ESC   H    J    K    L    Enter
Z    X    C    V    B    : | ;(3)  N    M(hold=_)  ,(hold=-)  .(hold=?)  @(hold=Alt)
Ctrl Alt  Win  Lang2(7)  Shift  Lang1(3) | BS(2)  Space(1)  Del
```

## MOUSEレイヤー同期ルール

**デフォルトレイヤーを変更した場合は、必ずMOUSEレイヤー（layer 4）も同じ内容に更新すること。**

### ルール

- MBボタン（`&mkp_exit_AML MB1/2/3/4/5`）はそのまま維持
- 例外として、`MB2` は scroll layer 5 に入る Windows 用の派生バインドを許可する。該当する場合は `&mkp_exit_scroll MB2` のように別マクロで明示する。
- ジェスチャーキー（`&mouse_gesture_kp N KEY`）はそのまま維持
- それ以外の全キーは `&to_layer_0 KEY` 形式でデフォルトレイヤーのタップキーと一致させる
- タップキーがないキー（`&mo N` 等）は `&trans` にする

### 対応表の例

| デフォルトレイヤー | MOUSEレイヤー |
|---|---|
| `&kp X` | `&to_layer_0 X` |
| `&mt MOD X` (tap=X) | `&to_layer_0 X` |
| `&lt N X` (tap=X) | `&to_layer_0 X` |
| `&sk X` | `&to_layer_0 X` |
| `&mo N` (タップなし) | `&trans` |
| `&mouse_gesture_kp N KEY` | そのまま維持 |
| `&mkp_exit_AML MBx` | そのまま維持 |
| `&mkp_exit_scroll MB2` | Windows 用派生。MB2 長押しで scroll layer 5 に移動 |

### Windows 用の差分ルール

- 修飾キーの見た目は Windows 配列に合わせる。左下の並びは `Ctrl / Win / Alt` を基本にする。
- `LG(...)` は Mac の `Cmd` として使っているので、Windows 版では原則 `LC(...)` または `LC(LG(...))` に置き換える。
- タブ操作、コピー/貼り付け、仮想デスクトップ切替などは Windows の標準ショートカットに合わせて調整する。
- `MB2` は scroll layer 5 を優先する。デフォルトの MOUSE レイヤーと同じ見た目を保ちつつ、Windows 版ではこの挙動差を許容する。

## コミット＆プッシュ

変更後は対象ブランチにコミットしてプッシュする:
```bash
git add config/roBa.keymap  # または変更したファイル
git commit -m "変更内容の説明"
git push origin <branch>
```
