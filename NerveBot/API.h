#pragma once
#include "json.hpp"
using json = nlohmann::json;
// ��ȡ���н��׶��б�
std::string GetTradings();
//��ȡ��ַ�����ʲ�����nonceֵ
std::string GetLedger(std::string Address, std::string Asset);
//��ȡָ����������
std::string GetOrderInfo(std::string OrderId);
//��ѯ����ί�е��б�
//Type ί�е����� 1���򵥣�2������
//State ����״̬ 0:ȫ����1���ҵ��У�2:���ֳɽ���3:�ѳɽ� ��4�ѳ�����5�����ֳɽ��ѳ�����
std::string GetOrderList(std::string Address, std::string symbol, int Type, int State);
//��ȡָ�����׶��̿���Ϣ
std::string GetOrderBook(std::string Symbol, std::string ItemNum);
//��ȡָ�����׶Ե�Hash
std::string GetTradingHash(std::string TradingName);
//ͨ��Hash��ȡ���׶Ե���ϸ��Ϣ
nlohmann::json GetTradingInfo(std::string Hash);
//������װί�е����� ��ȡ����txHex
//Type 1:�� 2:����
std::string GetMameOrderHash(std::string Address, float Quantity, float Price, std::string Trading, int Type);
//�㲥����
std::string Broadcast(std::string Hash);
//����ת�˽���
std::string Transfer();
//������װɾ����������
std::string GetCancelOrderHash(std::string Hash);
//��ȡ�ʲ������Ϣ
json getAssetInfo(std::string Asset);
//��ȡ�˻��ʲ���Ϣ,available,freeze,nonce
json GetAccountLedgerInfo(std::string Asset);
//��ȡ������װ���׵�hash
json CreateTransferTxOffline(std::string TransferStr);
//JsonRPC
json JsonRPC(std::string Method, json Params);