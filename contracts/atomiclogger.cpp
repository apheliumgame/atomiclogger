#include "atomiclogger.hpp"
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>

checksum256 atomiclogger::get_tx_id()
{
    auto size = transaction_size();
    char buf[size];
    uint32_t read = read_transaction(buf, size);
    check(size == read, "read_transaction failed");
    return sha256(buf, read);
}

void atomiclogger::saveTransfer(name collection_name, name from, name to, vector <uint64_t> asset_ids, string memo)
{
    auto itr = transferlogs.emplace(get_self(), [&](auto& row) {
        row.key = transferlogs.available_primary_key();
        row.tx = get_tx_id();
        row.collection_name = collection_name;
        row.from = from;
        row.to = to;
        row.assets_ids = asset_ids;
        row.memo = memo;
        row.created_at = eosio::current_time_point();
    });
}

void atomiclogger::saveMint (
        uint64_t asset_id,
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner
)
{
    auto itr = mintlogs.emplace(get_self(), [&](auto& row) {
        row.key = mintlogs.available_primary_key();
        row.asset_id = asset_id;
        row.authorized_minter = authorized_minter;
        row.collection_name = collection_name;
        row.schema_name = schema_name;
        row.template_id = template_id;
        row.new_asset_owner = new_asset_owner;
        row.created_at = eosio::current_time_point();
        row.tx = get_tx_id();
    });
}

void atomiclogger::saveBurn (
        name asset_owner,
        uint64_t asset_id,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name asset_ram_payer
)
{
    auto itr = burnlogs.emplace(get_self(), [&](auto& row) {
        row.key = burnlogs.available_primary_key();
        row.asset_owner = asset_owner;
        row.asset_id = asset_id;
        row.collection_name = collection_name;
        row.schema_name = schema_name;
        row.template_id = template_id;
        row.asset_ram_payer = asset_ram_payer;
        row.created_at = eosio::current_time_point();
        row.tx = get_tx_id();
    });
}

void atomiclogger::deltransrow(uint64_t key)
{
    check(has_auth(get_self()), "Not authorized");
    auto itr = transferlogs.find(key);
    check(itr != transferlogs.end(), "Record not found");

    transferlogs.erase(itr);
}

void atomiclogger::delmintrow(uint64_t key)
{
    check(has_auth(get_self()), "Not authorized");
    auto itr = mintlogs.find(key);
    check(itr != mintlogs.end(), "Record not found");
    
    mintlogs.erase(itr);
}

void atomiclogger::delburnrow(uint64_t key)
{
    check(has_auth(get_self()), "Not authorized");
    auto itr = burnlogs.find(key);
    check(itr != burnlogs.end(), "Record not found");
    
    burnlogs.erase(itr);
}
