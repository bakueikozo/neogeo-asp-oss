# neogeo-asp-oss

NEOGEO ASPのOSSソースコードの妥当性をチェックするためのリポジトリです。
各ディレクトリ内のファイルは
https://github.com/xudarren/NeogeoASP_OSS
からダウンロードして登録したものです。
各ディレクトリ内の著作権情報及びライセンス情報はコピー元をそのまま登録したもので、妥当なものかどうかの保証はありません。あくまでもデータ。
ASP-Emulator(mvsnjemu)のコピー元と思われるnjemuのリポジトリはここ
https://github.com/phoe-nix/NJEMU

なんか気が付いたことがあったらIssuesに書いてね

ASP-Emulatorのzip  
zlib の変更はPSP_Hの定義変更のみ  
zip/ の変更はsceIO系のファイルをstdioのfopen系に変更したのみ  

snk $ diff NJEMU/src/common/ source/ASP-Emulator/src/common/ -q -s  
NJEMU/src/common/ のみに存在: adhoc.c  
NJEMU/src/common/ のみに存在: adhoc.h  
NJEMU/src/common/ のみに存在: cache.c  
ファイル NJEMU/src/common/cache.h と source/ASP-Emulator/src/common/cache.h は異なります  
source/ASP-Emulator/src/common/ のみに存在: cache.s  
NJEMU/src/common/ のみに存在: cmdlist.c  
NJEMU/src/common/ のみに存在: cmdlist.h  
NJEMU/src/common/ のみに存在: coin.c  
NJEMU/src/common/ のみに存在: coin.h  
ファイル NJEMU/src/common/loadrom.c と source/ASP-Emulator/src/common/loadrom.c は異なります  
ファイル NJEMU/src/common/loadrom.h と source/ASP-Emulator/src/common/loadrom.h は異なります  
ファイル NJEMU/src/common/state.c と source/ASP-Emulator/src/common/state.c は異なります  
ファイル NJEMU/src/common/state.h と source/ASP-Emulator/src/common/state.h は異なります  
snk $
