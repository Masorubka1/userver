# /// [redis setup]
import json

import pytest

pytest_plugins = ['pytest_userver.plugins.aerospike']
# /// [aerospike setup]


# /// [service_env]
@pytest.fixture(scope='session')
def service_env(aerospike_sentinels):
    secdist_config = {
        'aerospike_settings': {
            'taxi-tmp': {
                'password': '',
                'sentinels': aerospike_sentinels,
                'shards': [{'name': 'test_master0'}],
            },
        },
    }

    return {'SECDIST_CONFIG': json.dumps(secdist_config)}
    # /// [service_env]
