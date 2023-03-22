#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>

using namespace std;
using namespace eosio;

CONTRACT atomiclogger : public contract {
    public:
        using contract::contract;

    checksum256 get_tx_id();

    [[eosio::on_notify("atomicassets::logtransfer")]] void saveTransfer (
        name collection_name,
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    [[eosio::on_notify("atomicassets::logmint")]] void saveMint (
        uint64_t asset_id,
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner
    );

    [[eosio::on_notify("atomicassets::logburnasset")]] void saveBurn (
        name asset_owner,
        uint64_t asset_id,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name asset_ram_payer
    );

    [[eosio::action]] void deltransrow (uint64_t key);
    [[eosio::action]] void delmintrow (uint64_t key);
    [[eosio::action]] void delburnrow (uint64_t key);

    private:
        struct [[eosio::table]] transferlog {
            uint64_t key;
            name collection_name;
            name from;
            name to;
            vector <uint64_t> assets_ids;
            string memo;
            time_point_sec created_at;
            checksum256 tx;

            uint64_t primary_key() const { return key;}
        };
        typedef multi_index<name("transferlogs"), transferlog> transferlog_s;

        transferlog_s transferlogs = transferlog_s(get_self(), get_self().value);

        struct [[eosio::table]] mintlog {
            uint64_t key;
            uint64_t asset_id;
            name authorized_minter;
            name collection_name;
            name schema_name;
            int32_t template_id;
            name new_asset_owner;
            time_point_sec created_at;
            checksum256 tx;

            uint64_t primary_key() const { return key;}
        };
        typedef multi_index<name("mintlogs"), mintlog> mintlog_s;

        mintlog_s mintlogs = mintlog_s(get_self(), get_self().value);

        struct [[eosio::table]] burnlog {
            uint64_t key;
            name asset_owner;
            uint64_t asset_id;
            name collection_name;
            name schema_name;
            int32_t template_id;
            name asset_ram_payer;
            time_point_sec created_at;
            checksum256 tx;

            uint64_t primary_key() const { return key;}
        };
        typedef multi_index<name("burnlogs"), burnlog> burnlog_s;

        burnlog_s burnlogs = burnlog_s(get_self(), get_self().value);
};
