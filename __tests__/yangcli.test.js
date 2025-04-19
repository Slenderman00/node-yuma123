import { NCX_DISPLAY_MODE_XML_NONS, yangcli, safeConnect, yuma123 } from '../index.js';
import {jest} from '@jest/globals';

describe('NETCONF Connection Tests', () => {
  let connection;
  const server = "127.0.0.1";
  const port = 830;
  const username = "pi";
  const password = "raspberry";
  
  beforeEach(() => {
    connection = safeConnect(server, port, username, password, null, null, null);
  });
  
  afterEach(() => {
    if (connection) {
      try {
        yuma123.yangrpc.close(connection);
      } catch (error) {
        if (!error.message.includes("Connection already closed")) {
          throw error;
        }
      }
    }
    jest.clearAllMocks();
  });
  
  test('should successfully connect and retrieve root data', () => {
    const result = yangcli(connection, "xget /", NCX_DISPLAY_MODE_XML_NONS);
    
    expect(result).not.toBeNull();
    expect(typeof result).toBe('string');
  });
  
  test('should throw an error when fed bad data', () => {
    expect(() => {
      safeConnect("bad-server", port, username, password, null, null, null);
    }).toThrow();
  });
  
  test('should close connection properly', () => {
    const closeSpy = jest.spyOn(yuma123.yangrpc, 'close');
    
    yuma123.yangrpc.close(connection);
    expect(closeSpy).toHaveBeenCalledWith(connection);
    
    closeSpy.mockRestore();
  });

  test('this test should always fail', () => {
    expect(true).toBe(false);
  });
});