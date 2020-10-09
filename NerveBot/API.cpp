#include "stdafx.h"
#include <string>
#include "libcurl.h"
#include "StringConvert.h"
#include "API.h"
#include "json.hpp"
#include "Language.h"
#include "Config.h"
#include <curl/curl.h>
#include "JsonParse.h"
#include "Logs.h"

extern fcurl_global_init		pcurl_global_init;
extern fcurl_global_cleanup	pcurl_global_cleanup;


using json = nlohmann::json;
//std::string PublicServer = "https://public.nerve.network/api";
std::string PublicServer = "http://beta.public.nerve.network/api";

//std::string PublicAPI = "https://api.nerve.network/jsonrpc/";
std::string PublicAPI = "http://beta.api.nerve.network/jsonrpc/";

//std::string APIURL = "https://nervedex.com/";
std::string APIURL = "http://beta.nervedex.com/";

//int ChainId = 9;
int ChainId = 5;

//std::string MainPrefix = "NERVE";
std::string MainPrefix = "TNVT";

std::string UserPassword = "";//�û����룬���ڽ���˽Կ
std::string LibcurlPost(std::string PostData, std::string GetData) {
	std::string Result = "";
	
	Result = PostUrl(PostData, GetData);

	return Result;
}
// ��ȡ���н��׶��б�
std::string GetTradings() {
	std::string GetData = "coin/trading/config";
	return LibcurlPost("", GetData);
}
//��ȡ��ַ�����ʲ�����nonceֵ
std::string GetLedger(std::string Address, std::string Asset) {
	std::string GetData = "api/ledger/" + Address + "/" + Asset;
	return LibcurlPost("", GetData);
}

//��ȡָ����������
std::string GetOrderInfo(std::string OrderId) {
	std::string GetData = "api/order/" + OrderId;
	return LibcurlPost("", GetData);
}
//��ѯ����ί�е��б�
//Type ί�е����� 1���򵥣�2������
//State ����״̬ 0:ȫ����1���ҵ��У�2:���ֳɽ���3:�ѳɽ� ��4�ѳ�����5�����ֳɽ��ѳ�����
std::string GetOrderList(std::string Address, std::string symbol, int Type, int State) {
	std::string GetData = "api/order/list";
	std::string PostData = "{\"address\":\""+ Address +"\",";
	PostData += "\"pageNumber\": 1,";
	PostData += "\"pageSize\": 30,";
	PostData += "\"beginTime\": 1586663355909,";
	PostData += "\"endTime\": 99999999999999,";
	PostData += format("\"type\": %d,",Type);
	PostData += "\"symbol\": \""+ symbol +"\",";
	PostData += format("\"state\": %d}", State);
	return LibcurlPost(PostData, GetData);

}
//��ȡָ�����׶��̿���Ϣ
std::string GetOrderBook(std::string Symbol, std::string ItemNum) {
	std::string GetData = "api/orderBook/" + Symbol + "/" + ItemNum;
	return LibcurlPost("", GetData);
}
//��ȡ���׶Ե�config��Ϣ
std::string GetTradingConfig() {
	std::string GetData = "coin/trading/config";
	return LibcurlPost("", GetData);
}
//��ȡָ�����׶Ե�Hash
std::string GetTradingHash(std::string TradingName)
{
	std::string Result = "";
	std::string JsonStr = GetTradingConfig();
	json J;
	try {
		J = json::parse(JsonStr);
		for (auto&i : J["data"]["result"].items()) {
			for (auto&s : i.value()["tradingDtoList"].items()) {
				std::string FieldValue = s.value()["tradingName"].get<std::string>();
				//LogA("%s", FieldValue.c_str());
				if (s.value()["tradingName"].get<std::string>() == TradingName) {
					Result = s.value()["hash"].get<std::string>();
					return Result;
				}

			}
		}
	}
	catch (...) {
		std::string Caption = GetItemName("��ȡָ�����׶Ե�Hash�쳣");
		std::string Title = GetItemName("��ʾ");
		MessageBoxA(NULL, Caption.c_str(), Title.c_str(), MB_OK);
	}
	return Result;
}
//ͨ��Hash��ȡ���׶Ե���ϸ��Ϣ
nlohmann::json GetTradingInfo(std::string Hash) {
	json Result = "";
	std::string GetData = "coin/trading/get/" + Hash;
	std::string JsonStr = LibcurlPost("", GetData);
	json J;
	try {
		J = json::parse(JsonStr);
		Result = J["data"]["result"];
	}
	catch (...) {
		std::string Logs = "��ȡ���׶���ϸ��Ϣ�쳣��";
		Logs += "---" + GetData;
		Logs += "---" + JsonStr;
		LogsOut(Logs);
	}
	return Result;
}

//������װί�е����� ��ȡ����txHex
//Type 1:�� 2:����
std::string GetMameOrderHash(std::string Address, float Quantity, float Price, std::string Trading, int Type){
	std::string GetData = "api/order";
	std::string PostData = "{\"address\":\"" + Address + "\",";
	PostData += format("\"quantity\": %f,", Quantity);
	PostData += format("\"price\": %f,", Price);
	PostData += format("\"symbol\":\"%s\",", Trading.c_str());
	PostData += format("\"type\": %d}", Type);
	return LibcurlPost(PostData, GetData);
}
//������װɾ����������
std::string GetCancelOrderHash(std::string Hash) {
	std::string GetData = "api/cancelOrder";
	std::string PostData = format("{\"orderId\":\"%s\"}", Hash.c_str());
	return LibcurlPost(PostData, GetData);
}
//�㲥����
std::string Broadcast(std::string Hash) {
	std::string GetData = "api/broadcast";
	std::string PostData = format("{\"txHex\":\"%s\"}",Hash.c_str());
	return LibcurlPost(PostData, GetData);
}
//ת�˽���
std::string Transfer() {
	std::string  PostData = "";
	PostData += "{\"jsonrpc\" : \"2.0\",\"method\" : \"createTransferTxOffline\",";
	PostData += "\"params\" : [ [ {";
	PostData += format("\"address\" : \"TNVTdTSPGQc9eSCkUbnwVyJuBCaM4Zj2PVpvN\",\"assetChainId\" : 5,\"assetId\" : 1,\"amount\" : \"1000000000\",\"nonce\" : \"0000000000000000\"");
	PostData += "} ], [ {";
	PostData += format("\"address\" : \"TNVTdTSPGQc9eSCkUbnwVyJuBCaM4Zj2PVpvN\",\"assetChainId\" : 5,\"assetId\" : 1,\"amount\" : \"1000000000\",\"lockTime\" : 0");
	PostData += " } ], \"222\" ],\"id\" : 2530}";
	std::string JsonStr = LibcurlPost(PostData, PublicAPI);
	return JsonStr;
}
//��ȡ�ʲ������Ϣ,AssetsChainId,AssetsId,decimals
json getAssetInfo(std::string Asset) {
	json Return = "";
	std::string  PostData = "";
	PostData += "{\"jsonrpc\" : \"2.0\",\"method\" : \"getAssetList\",";
	PostData += format("\"params\" : [%d,0],\"id\" : 2530}",ChainId);
	std::string JsonStr = LibcurlPost(PostData, PublicServer);
	if (Asset == "") {
		try
		{
			Return = json::parse(JsonStr);
		}
		catch (...)
		{

		}
	}
	else {
		Return = GetJsonByField(JsonStr, "result", "symbol", Asset);
	}
	
	return Return;
}
//��ȡ�˻��ʲ���Ϣ,available,freeze,nonce
json GetAccountLedgerInfo(std::string Asset) {
	json Return = "";
	std::string JsonStr = GetLedger(GetConfig("Address"), Asset);
	try
	{
		json J = json::parse(JsonStr);
		Return = J["data"];
	}
	catch (...)
	{

	}
	return Return;
}
//��ȡ������װ���׵�hash
json CreateTransferTxOffline(std::string TransferStr) {
	json Return = "";
	std::string JsonStr = LibcurlPost(TransferStr, PublicAPI);
	try
	{
		Return = json::parse(JsonStr);

	}
	catch (...)
	{

	}
	return Return;
}
//JsonRPC
json JsonRPC(std::string Method,json Params) {
	json Return = "";
	json JsonPost;
	JsonPost["jsonrpc"] = "2.0";
	JsonPost["method"] = Method;
	JsonPost["params"] = Params;
	JsonPost["id"] = "2530";

	std::string JsonStr = LibcurlPost(JsonPost.dump(), PublicAPI);
	try
	{
		Return = json::parse(JsonStr);

	}
	catch (...)
	{

	}
	return Return;
}
//ͨ��ChainId��assetId��ȡ����nonce

