import pytest

import requests_client


@pytest.mark.skip(reason='Flaky TAXICOMMON-7319')
@pytest.mark.parametrize('case', requests_client.ALL_CASES)
async def test_close_connection(grpc_ch, service_client, gate, case):
    await requests_client.close_connection(gate, grpc_ch)
    await requests_client.check_200_for(case)(grpc_ch, service_client, gate)
