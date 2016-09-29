Drupal.behaviors.membership_admin = function(context) {
  // Set up click events on the display selector checkboxes.
  $.each(['status', 'sort', 'identity', 'contact', 'emergency'], function(index, value) {
    $('#' + value, context).click(function() {
      if ($(this).attr('checked')) {
        $('.' + value).show();
        setSessionVariable(value, 1);
      }
      else {
        $('.' + value).hide();
        setSessionVariable(value, 0);
      }
    });
  });

  $('.emergency').hide();
  $('.sort').hide();
  $('.contact').hide();
  $('#status').attr('checked', 'checked');
  $('#identity').attr('checked', 'checked');

  // Set up select-all
  $('#select-all', context).click(function() {
    if ($(this).attr('checked')) {
      $('.selection').attr('checked', 'checked');
    }
    else {
      $('.selection').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Set up unselect all when one checkbox is unselected.
  $('.selection', context).click(function() {
    if (!$(this).attr('checked')) {
      $('#select-all').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Disable the 'Action' dropdown if there is no selection.
  checkActionDropdownState();

  // Set up the actions on selected records.
  $('#run-action', context).click(function() {
    var command = $('#action').val();
    // Assemble a list of ids to send to the server with the command:
    var selected_members = $.map($('.selection:checked', context), function(element) {
      return element.value;
    });
    var post_data = {
      selected_members: selected_members.join(',')
    };

    $.post('/membership/update/' + command, post_data, function(result) {
      eval("output=" + result);
      if (output.result) {
        // Update the changed records in situ, or refresh the page.
        $(output.data).each(function(idx, val) {
          if (command == 'paid') {
            $('TD#paid-' + val).html(1);
          }
          else if (command == 'unpaid') {
            $('TD#paid-' + val).html(0);
          }
          else if (command == 'active') {
            $('TD#status-' + val).html('active');
          }
          else if (command == 'rejected') {
            $('TD#status-' + val).html('rejected');
          }
          else if (command == 'expired') {
            $('TD#status-' + val).html('expired');
          }
          else if (command == 'export') {
            // Output.data.val is the path of the download csv
            if (val.path) {
              $('iframe#dliframe').attr('src', '/membership/download?path=' + val.path);
            }
            else if (val.error) {
              alert(val.error);
            }
            else {
              alert('Something went wrong');
            }
          }
        });
      }
      else {
        alert("Failed " + command + " to get " + output.result + " with " + output.data.selected_members);
      }
    });
  });

  // Activate the run button when a valid action has been chosen
  $('#action', context).change(function() {
    if ($(this).val() == 'action') {
      $('#run-action').attr('disabled', true).attr('title', Drupal.t('Choose an action to run first.'));
    }
    else {
      $('#run-action').attr('disabled', false).attr('title', Drupal.t('Click here to run the chosen action against all the checked records below.'));
    }
  });
}

function checkActionDropdownState() {
  $('#action').attr('title', Drupal.t('Select the records you want to modify by checking their checkboxes.'));
  $('#action').attr('disabled', true);

  $('.selection').each(function() {
    if ($(this).attr('checked')) {
      $('#action').attr('disabled', false);
    }
  });

  $('#action:enabled').attr('title', Drupal.t('Choose your action then click the Run button.'));
  if ($('#action:enabled').size() && $('#action').val() != 'action') {
    $('#run-action').attr('disabled', false).attr('title', Drupal.t('Click here to run the chosen action against all the checked records below.'));
  }
  else {
    $('#run-action').attr('disabled', true).attr('title', Drupal.t('Choose an action to run first.'));
  }
}

function setSessionVariable(name, value) {
  var post_data = {
    name: value
  };

  //$.post('/membership/update/' + command, post_data, function(result) {
  $.ajax({
    type: 'POST',
    data: name + '=' + value,
    url: Drupal.settings.basePath + 'admin/ajax/set/' + name,
    dataType: 'json',
    success: function(result) {
      alert("Got result");
    },
  });
}

function getSessionVariable(name) {
}
