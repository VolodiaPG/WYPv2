import { Injectable, Input, EventEmitter, Output } from '@angular/core';
import { WebSocketSubject } from 'rxjs/internal/observable/dom/WebSocketSubject';
import { WebSocketModule, Commands_t } from './websocket-module'
import { sha256 } from 'js-sha256';

@Injectable({
  providedIn: 'root'
})

export class WebsocketWrapperService {

  @Output() public moduleAdded: EventEmitter<WebSocketModule> = new EventEmitter();

  public modules: WebSocketModule[];
  public parameterReceived: EventEmitter<{ module: WebSocketModule, commands: number }> = new EventEmitter();

  private websocket = new WebSocket('ws://192.168.1.32/ws', ['arduino']);

  constructor() {
    var self = this;

    this.websocket.onopen = function (event) {
      self._OnOpen(self, event);
    };
    this.websocket.onclose = function (event) {
      self._OnClose(self, event);
    };
    this.websocket.onerror = function (event) {
      self._OnError(self, event);
    };
    this.websocket.onmessage = function (event) {
      self._OnMessage(self, event);
    };

    this.modules = [];
  }

  Send(to: number, password: string, password_salt: number, salt: number, message: Object) {
    let json = {};
    json['to'] = to;
    json['from'] = 0; //coming from ws client
    let args = json['arguments'] = message;

    let a = JSON.stringify(args);
    let str = password + password_salt + salt + a;
    let cs = sha256(str);

    json['checksum'] = cs;

    console.log('To', to, '=>', args);

    this.websocket.send(JSON.stringify(json));
  }

  private _OnOpen(self: WebsocketWrapperService, event: Event) {
    console.log(event);
    this.modules = [];
  }

  private _OnClose(self: WebsocketWrapperService, event: CloseEvent) {
    console.log(event);
  }

  private _OnError(self: WebsocketWrapperService, event: Event) {
    console.log(event);
  }

  private _OnMessage(self: WebsocketWrapperService, event: MessageEvent) {

    let data = JSON.parse(event.data);

    if (data) {
      console.log('From', data['from'], '=>', data['arguments']);

      let mod = this.GetModule(data['from']);


      let added = false;

      if (!mod) {
        mod = this._AddToModules(data['from']); 
        added = true;
      }

      mod.pass_salt = data['passsalt'];
      mod.salt = data['salt'];

      if (added) {
        this.moduleAdded.emit(mod);
      }

      let err = data['error'];
      if (err) {
        console.log('error:', err);
      }

      let args = data['arguments']

      if (args['meshclts'] !== undefined) {
        var modulesGone = this.modules;

        args['meshclts'].forEach(element => {
          
          let ret = this._AddToModules(element);
          // if (modulesGone.indexOf(ret) != -1) {
          //   modulesGone.splice(modulesGone.indexOf(ret), 1);
          // }

          //here because otherwise the salt ins't registered before a new message can be send through the event
          this.moduleAdded.emit(ret);
        });

        // if (modulesGone.length != 0) {
        //   modulesGone.forEach(element => {
        //     console.log('module removed');
        //     this.modules.splice(this.modules.indexOf(element), 1);
        //   });
        // }
      }

      console.log('modules', this.modules);

      var p: number = 0;

      for (var key in Commands_t) {
        if (args[key] !== undefined) {
          console.log(key);
          mod[key] = args[key];
          p |= +Commands_t[key];
        }
      }

      if (p !== 0) {
        this.parameterReceived.emit({ module: mod, commands: p });
      }

    }
  }

  private

  private _AddToModules(nodeId: number): WebSocketModule {
    console.log(nodeId);
    var ret = undefined;
    for (var element of this.modules) {
      if (element.nodeId === nodeId) {
        ret = element;
        break;
      }
    }

    if (!ret) {
      ret = new WebSocketModule(this, nodeId);
      this.modules.push(ret);
    }
    return ret;
  }

  GetModule(nodeId: number): WebSocketModule {
    var ret = undefined;
    for (var element of this.modules) {
      if (element.nodeId === nodeId) {
        ret = element;
        break;
      }
    }
    return ret;
  }

  GetModules(): WebSocketModule[] {
    return this.modules;
  }
}
