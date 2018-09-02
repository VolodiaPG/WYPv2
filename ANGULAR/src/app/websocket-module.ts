import { WebsocketWrapperService } from "./websocket-wrapper.service";

export enum Commands_t {
    relay = 1 << 0,
    time = 1 << 1,
    uptime = 1 << 2,
    timezone = 1 << 3,
}

export class WebSocketModule {
    //methods
    constructor(websocket: WebsocketWrapperService, id: number) {
        this._nodeId = id;
        this._websocket = websocket;
    }

    Refresh() {
        this._websocket.Send(this._nodeId, this.password, this.pass_salt, this.salt, { 'relay': '' });
    }

    SetRelay(value: boolean) {
        // this.relayState = value;
        this._websocket.Send(this._nodeId, this.password, this.pass_salt, this.salt, { 'relay': value });
    }

    Send(value: Object) {
        this._websocket.Send(this._nodeId, this.password, this.pass_salt, this.salt, value);
    }

    get nodeId(): number {
        return this._nodeId;
    }

    Request(arg: number): void {
        var args = {};

        for (var key in Commands_t) {
            if (arg & +Commands_t[key]) {
                args[key] = '';
            }
        }

        if (arg) {
            this._websocket.Send(this._nodeId, this.password, this.pass_salt, this.salt, args);
        }
    }

    GetUptimeFormatted(): string {
        var s = this.uptime;

        // Pad to 2 or 3 digits, default is 2
        function pad(n, z = 2) {
            z = z || 2;
            return ('00' + n).slice(-z);
        }

        var ms = s % 1000;
        s = (s - ms) / 1000;
        var secs = s % 60;
        s = (s - secs) / 60;
        var mins = s % 60;
        var hrs = (s - mins) / 60;

        return pad(hrs) + ':' + pad(mins) + ':' + pad(secs) + '.' + pad(ms, 3);

    }

    SetTimezone(value: number): void {
        this.timezone = value;
        this.Send({ 'timezone': value });
    }

    //variables
    private _nodeId: number;
    private _websocket: WebsocketWrapperService;

    private password = 'awesomepass';

    public pass_salt: number;
    public salt: number;
    public relay: boolean = false;

    public time: number;
    public uptime: number;
    public timezone: number;

}