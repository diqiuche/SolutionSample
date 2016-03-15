#ifndef WORKPLAN_TASK_INTERFACE_DEF_H_
#define WORKPLAN_TASK_INTERFACE_DEF_H_

#include <string>
#include <stdio.h>
#include <string.h>
#pragma pack(push,1)     /*指定1字节对齐*/

//错误码定义
#define	PROCESS_SUCCESS				  0	    //处理成功
#define	REQUEST_PACKAGE_ILLEGAL		  1		//请求数据异常
#define	OPERATE_DB_FAILED			  2		//数据库操作失败
#define	USRID_EXISTED				  3		//登录账号已经存在
#define	USRID_NOT_EXISTED			  4		//未注册的手机号
#define	WRONG_PASSWD				  5		//登录密码错误
#define	MAC_ADDRESS_CHANGE			  6		//mac地址变化
#define CMD_NOT_SUPPORTED			  7		//不支持请求
#define	INFO_NOT_EXIST				  8		//无相关信息
#define	NOT_LOGIN_YET				  9		//未登录
#define	ME_ORDER_NOTOVER			 10		//还有融资订单未处理
#define	ME_UP_LIMIT					 11		//超过融资上限
#define	ME_CASH_USEUP				 12		//暂无资金可融资
#define	ME_ORDER_INVALID			 13		//融资订单无效
#define	ME_PAY_LESS					 14		//实际支付金额小于应该支付金额
#define	RISK_CTRL_NOT_PASS			 15		//风控检查未通过
#define	BUY_FORBIDDEN				 16		//没有买入权限
#define	SELL_FORBIDDEN				 17		//没有卖出权限
#define	WARRANT_RATIO_NOT_PASS		 18		//担保比率不符合要求
#define	SEC_MV_NOT_PASS				 19		//证券市值不符合要求
#define	BELONGTO_BLACK_LIST			 20		//黑名单证券代码
#define	MD_SERVER_HALT				 21		//实时行情服务终止
#define	GET_ASSET_FAILED			 22		//获取资产失败
#define ASSET_NOT_ENOUGH			 23		//资产不足
#define	SLEF_SELECTION_EXISTED		 24		//自选股已存在
#define SLEF_SELECTION_NOT_EXISTED	 25		//自选股不存在
#define	SLEF_OVER_MAX_CASH   		 26		//额度超过最大最
#define	ME_ORDER_FINISH_PZTY	     27		//体验配资未完结
#define	ME_ORDER_NOPOWER_PZTY	     28		//没有体验配资资格
#define	PUSH_MONEY_FAILED	         29		//入金失败
#define	DRAW_MONEY_NOT_ENOUGH	     30		//可提余额不足
#define	DRAW_MONEY_FAILED	         31		//出金失败
#define	WARRAN_TRATE_MIN	         32		//担保比例低于0.6，请充值后再尝试展期
#define	LEFT_MONEY_NOT_ENOUGH	     33		//余额不足支付利息
#define	USED_MONEY_NOT_ENOUGH	     34		//可用资金不足支付利息
#define ERR_TABLE_BASE_DATE			 35		//基准日期表配置错误
#define NOT_TRADING_TIME			 36		//非交易时间
#define NOT_SUPPORT_ENTRUST			 37		//当前时间点不支持的委托
#define	ENTRUST_CANCEL_FORBIDDEN	 38		//没有撤单权限
#define	ENTRUST_INVALID				 39		//委托无效
#define	CONDITION_ORDER_INVALID		 40		//条件单已经失效
#define	PUSH_MONEY_NO_PRIVILEGE		 41		//无入金权限
#define	DURING_PZTY_NO_PRIVILEGE	 42		//体验期间，不开放入金权限
#define	ADD_INFO_EXISTED	         43		//添加记录已存在
#define	ORDER_CANNOT_CANCEL	         44		//该单不可撤单
#define	FUND_ACCOUNT_CANNOT_FIND	 45		//找不到资金账户
#define	INSTRUMENT_INFO_CANNOT_FIND	 46		//找不到合约参数
#define	RISKRULE_INFO_CANNOT_FIND	 47		//找不到风控规则
#define	MARGIN_RATIO_NOT_PASS		 48		//保证金比率不符合要求
#define	QUANTITY_USE_NOT_ENOUGH		 49		//可平数量不足
#define	USER_LOGIN_NO_PRIVILEGE		 50		//该用户没有登入权限
#define	ADMIN_CANNOT_DELETE			 51		//管理员不可以删除
#define	SYSROLE_CANNOT_ADD			 52		//系统角色不可以添加
#define	SYSROLE_CANNOT_MODIFY		 53		//系统角色不可以修改
#define	SYSROLE_CANNOT_DELETE		 54		//系统角色不可以删除
#define	USER_NO_ENTRUST_PRIVILEGE    55		//用户没有委托权限
#define	INST_IS_NOT_LEGAL		     56		//指令不合法
#define	PZACC_CANNOT_CHANGE_FUND	 57		//已分配资金的子帐不可更改资金账户
#define	EXIST_SAME_RISKCTRL_ITEM	 58		//已存在相同的风控指标
#define	SUBENTRUST_NO_CANCEL		 59		//指令相关的委托不符合撤单条件
#define	PRODUCT_NO_DEFAULT_ACC		 60		//产品没有默认子账户
#define	INST_TRADER_EXISTING		 61		//指令已有交易员
#define	GET_STOCK_MAXNUM_BUY_FAIL	 62		//获取股票最大可买数量失败
#define	DURING_SETTLEMENT_NO_OP		 63		//盘后结算期间不允许此操作
#define	PRODUCT_FUNDACC_EXISTED		 64		//产品下已存在同类资金账户

#define STOCK_SECURITY_TYPE          "1"    //股票
#define FUTURE_SECURITY_TYPE         "2"    //期货


#ifndef _WIN32
typedef unsigned char       BYTE;
#endif

//接口协议定义
//通用数据部分头
typedef struct tagResponseHead
{
	int success;		         //0-成功
	int requestid;		         //类别
	int num;			         //数量
}RESPONSE_HEAD,MULTI_MSGS_HEAD;

//1、注册请求【datatype=1】
typedef struct tagRegister
{
	int num;
}APP_REGISTER;

//2、登录请求【datatype=2】
typedef struct tagLogin
{
	int requestid;	//1--初次登录 2--验证登录 3--前置机登录

	char mobile[16];
	char password[64];
	char remote_ip[16];
	char macAddress[48];

	char reserve[128];		//预留
}APP_LOGIN;

typedef struct tagMemberIdRep
{
	int success;		//0-成功
	int member_id;		//会员id
}APP_MEMBER_ID_REP;

#pragma pack (pop)      /*还原默认字节对齐*/

#endif
